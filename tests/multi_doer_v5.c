// multi_doer_v5.c
// CMD v5: 支持 spawn 的 Doer → Doer 消息宇宙
// 编译：gcc -std=c11 -Wall -Wextra -O2 multi_doer_v5.c -o multi_doer_v5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ===== 基本常量 ===== */
#define MAX_DOERS  16   // 最多多少个 doer
#define NAME_LEN   31
#define PAYLOAD_LEN 127
#define QMAX       64

/* ===== Capability ===== */
typedef struct {
    int id;
} Capability;

/* ===== Message ===== */
typedef struct {
    char from[NAME_LEN + 1];
    char payload[PAYLOAD_LEN + 1];
} Message;

/* ===== 简单队列（单线程，无锁） ===== */
typedef struct {
    Message q[QMAX];
    int head, tail;
} MsgQueue;

static void q_init(MsgQueue *mq) { mq->head = mq->tail = 0; }
static bool q_empty(MsgQueue *mq) { return mq->head == mq->tail; }

static bool q_push(MsgQueue *mq, const Message *m) {
    int next = (mq->tail + 1) % QMAX;
    if (next == mq->head) return false; // full
    mq->q[mq->tail] = *m;
    mq->tail = next;
    return true;
}

static bool q_pop(MsgQueue *mq, Message *out) {
    if (q_empty(mq)) return false;
    *out = mq->q[mq->head];
    mq->head = (mq->head + 1) % QMAX;
    return true;
}

/* ===== Doer：行动主体 ===== */
typedef struct Doer {
    char name[NAME_LEN + 1];
    Capability cap;
    MsgQueue inbox;

    void (*handle)(struct Doer *, const Message *);
} Doer;

/* Doer 的默认行为：打印收到的消息 */
static void doer_handle(Doer *d, const Message *m) {
    printf("[Doer %s / cap=%d] from %s: %s\n",
           d->name, d->cap.id, m->from, m->payload);
}

/* 初始化一个 Doer */
static void doer_init(Doer *d, const char *name, int cap_id) {
    strncpy(d->name, name, NAME_LEN);
    d->name[NAME_LEN] = '\0';

    d->cap.id = cap_id;
    q_init(&d->inbox);
    d->handle = doer_handle;
}

/* 在当前 doer 列表中按名字查找 */
static Doer *find_doer(Doer **doers, int count, const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(doers[i]->name, name) == 0) {
            return doers[i];
        }
    }
    return NULL;
}

/* ===== Scheduler：把所有 inbox 清空一轮 ===== */
static void scheduler_drain(Doer **doers, int count) {
    Message m;
    for (int i = 0; i < count; i++) {
        while (q_pop(&doers[i]->inbox, &m)) {
            doers[i]->handle(doers[i], &m);
        }
    }
}

/* ===== root 命令解析 ===== */
/*
    支持指令：

    spawn C           → 创建一个新 Doer C
    send A B hello    → A 给 B 发消息 "hello"
    exit              → root 广播 EXIT，主循环结束
    其他文本          → root 广播给所有 Doer
*/

static bool parse_and_dispatch(
    const char *line,
    Doer *root,
    Doer **doers,
    int *doer_count,
    int *next_cap_id)
{
    int count = *doer_count;

    // ---- exit ----
    if (strcmp(line, "exit") == 0) {
        Message ex = {0};
        strncpy(ex.from, root->name, NAME_LEN);
        strncpy(ex.payload, "EXIT", PAYLOAD_LEN);

        for (int i = 0; i < count; i++) {
            q_push(&doers[i]->inbox, &ex);
        }
        return false; // 让主循环退出
    }

    // ---- spawn NAME ----
    if (strncmp(line, "spawn ", 6) == 0) {
        char new_name[NAME_LEN + 1];

        if (sscanf(line + 6, "%31s", new_name) == 1) {
            if (strcmp(new_name, root->name) == 0) {
                printf("[root] 不能使用保留名字 '%s'\n", root->name);
                return true;
            }
            if (find_doer(doers, count, new_name)) {
                printf("[root] Doer '%s' 已存在\n", new_name);
                return true;
            }
            if (count >= MAX_DOERS) {
                printf("[root] 已达到 MAX_DOERS=%d，不能再 spawn\n", MAX_DOERS);
                return true;
            }

            Doer *d = malloc(sizeof(Doer));
            if (!d) {
                perror("malloc");
                return true;
            }

            doer_init(d, new_name, (*next_cap_id)++);
            doers[count] = d;
            *doer_count = count + 1;

            printf("[root] spawned Doer '%s' (cap=%d)\n",
                   d->name, d->cap.id);
            return true;
        }
    }

    // ---- send SRC DST msg ----
    if (strncmp(line, "send ", 5) == 0) {
        char src[NAME_LEN + 1];
        char dst[NAME_LEN + 1];
        char msg[PAYLOAD_LEN + 1];

        if (sscanf(line + 5, "%31s %31s %127[^\n]", src, dst, msg) == 3) {
            Doer *fromD = NULL;
            if (strcmp(src, root->name) == 0) {
                fromD = root;
            } else {
                fromD = find_doer(doers, count, src);
            }

            Doer *toD = find_doer(doers, count, dst);

            if (!fromD) {
                printf("[root] send: 源 Doer '%s' 不存在\n", src);
                return true;
            }
            if (!toD) {
                printf("[root] send: 目标 Doer '%s' 不存在\n", dst);
                return true;
            }

            Message m = {0};
            strncpy(m.from, fromD->name, NAME_LEN);
            strncpy(m.payload, msg, PAYLOAD_LEN);

            if (!q_push(&toD->inbox, &m)) {
                printf("[root] inbox of '%s' is full\n", toD->name);
            }
            return true;
        }
    }

    // ---- 否则：root 广播 ----
    Message b = {0};
    strncpy(b.from, root->name, NAME_LEN);
    strncpy(b.payload, line, PAYLOAD_LEN);

    for (int i = 0; i < count; i++) {
        if (!q_push(&doers[i]->inbox, &b)) {
            printf("[root] inbox of '%s' is full\n", doers[i]->name);
        }
    }

    return true;
}

/* ===== main：根循环 ===== */

int main(void) {
    Doer root;
    doer_init(&root, "root", 1);

    Doer *doers[MAX_DOERS];
    int doer_count = 0;
    int next_cap_id = 2; // A=2, B=3, 后面依次递增

    // 先创建 A / B 两个初始 worker
    Doer *A = malloc(sizeof(Doer));
    Doer *B = malloc(sizeof(Doer));
    if (!A || !B) {
        perror("malloc");
        return 1;
    }
    doer_init(A, "A", next_cap_id++);
    doer_init(B, "B", next_cap_id++);

    doers[doer_count++] = A;
    doers[doer_count++] = B;

    printf("=== CMD v5: spawn-able Doer Universe ===\n");
    printf("指令示例：\n");
    printf("  spawn C              → 创建 Doer C\n");
    printf("  send A B hello tony  → A 给 B 发消息\n");
    printf("  send B A hi 大哥     → B 给 A 发消息\n");
    printf("  send C A yo          → C 给 A 发消息（先 spawn C）\n");
    printf("  任意文本             → root 广播给所有 Doer\n");
    printf("  exit                 → 广播 EXIT 并退出\n\n");

    char buf[256];

    for (;;) {
        printf(">> ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\n[root] EOF，退出\n");
            break;
        }
        buf[strcspn(buf, "\n")] = 0;

        bool cont = parse_and_dispatch(buf, &root, doers, &doer_count, &next_cap_id);

        scheduler_drain(doers, doer_count);

        if (!cont) {
            printf("=== CMD system exited ===\n");
            break;
        }
    }

    // 释放动态分配的 Doer
    for (int i = 0; i < doer_count; i++) {
        free(doers[i]);
    }

    return 0;
}
