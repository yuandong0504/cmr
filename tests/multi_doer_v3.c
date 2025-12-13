// multi_doer_v3.c
// CMD: 多 Doer + 显式 Scheduler 的最小实现
// 编译：gcc -std=c11 -Wall -Wextra -O2 multi_doer_v3.c -o multi_doer_v3
// 运行：./multi_doer_v3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ===== Capability ===== */
typedef struct {
    int id;
} Capability;

/* ===== Message ===== */
typedef enum {
    MSG_TEXT,
    MSG_EXIT
} MessageType;

typedef struct {
    MessageType type;
    char payload[128];
} Message;

/* ===== 简单队列（单线程，无锁） ===== */
#define QMAX 64

typedef struct {
    Message q[QMAX];
    int head, tail;
} MsgQueue;

static void q_init(MsgQueue *mq) {
    mq->head = mq->tail = 0;
}

static bool q_empty(MsgQueue *mq) {
    return mq->head == mq->tail;
}

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
typedef struct {
    const char *name;
    Capability cap;
    MsgQueue inbox;
} Doer;

static void doer_init(Doer *d, const char *name, int cap_id) {
    d->name = name;
    d->cap.id = cap_id;
    q_init(&d->inbox);
}

/* Doer 如何处理一条消息 */
static void doer_handle(Doer *d, const Message *msg) {
    switch (msg->type) {
    case MSG_TEXT:
        printf("[Doer %s / cap=%d] %s\n",
               d->name, d->cap.id, msg->payload);
        break;
    case MSG_EXIT:
        printf("[Doer %s] received EXIT\n", d->name);
        break;
    }
}

/* ===== Scheduler：专门负责“轮询所有 Doer 的 inbox” ===== */
typedef struct {
    Doer **doers;
    int count;
} Scheduler;

static void scheduler_init(Scheduler *s, Doer **doers, int count) {
    s->doers = doers;
    s->count = count;
}

/* 调度一步：从每个 doer 的 inbox 中各取一条消息处理 */
static bool scheduler_step(Scheduler *s) {
    bool handled_any = false;
    Message msg;

    for (int i = 0; i < s->count; i++) {
        Doer *d = s->doers[i];
        if (q_pop(&d->inbox, &msg)) {
            handled_any = true;
            doer_handle(d, &msg);
        }
    }
    return handled_any;
}

/* ===== root 输入解析：决定发给谁 ===== */
/*
  支持三种前缀：
    a xxx   → 发给 workerA
    b xxx   → 发给 workerB
    both xxx→ A/B 都收一份
  单独的 "exit" → 向所有 doer 发送 EXIT
*/

static void broadcast_exit(Doer **targets, int n) {
    Message m = { .type = MSG_EXIT, .payload = "" };
    for (int i = 0; i < n; i++) {
        q_push(&targets[i]->inbox, &m);
    }
}

static bool parse_and_dispatch_input(const char *line,
                                     Doer *workerA,
                                     Doer *workerB) {
    // exit → 让调用者结束主循环
    if (strcmp(line, "exit") == 0) {
        Doer *all[] = { workerA, workerB };
        broadcast_exit(all, 2);
        return false; // 告诉上层：该退出了
    }

    const char *rest = NULL;
    Doer *targets[2];
    int target_count = 0;

    if (strncmp(line, "a ", 2) == 0) {
        rest = line + 2;
        targets[0] = workerA;
        target_count = 1;
    } else if (strncmp(line, "b ", 2) == 0) {
        rest = line + 2;
        targets[0] = workerB;
        target_count = 1;
    } else if (strncmp(line, "both ", 5) == 0) {
        rest = line + 5;
        targets[0] = workerA;
        targets[1] = workerB;
        target_count = 2;
    } else {
        // 没有前缀，就轮流发给 A/B
        static int turn = 0;
        rest = line;
        targets[0] = (turn == 0) ? workerA : workerB;
        target_count = 1;
        turn ^= 1;
    }

    Message m;
    m.type = MSG_TEXT;
    strncpy(m.payload, rest, sizeof(m.payload));
    m.payload[sizeof(m.payload) - 1] = '\0';

    for (int i = 0; i < target_count; i++) {
        if (!q_push(&targets[i]->inbox, &m)) {
            fprintf(stderr, "[root] inbox for %s is full\n",
                    targets[i]->name);
        }
    }

    return true; // 继续主循环
}

/* ===== 主循环：root 输入 + scheduler 调度 ===== */

static void run_loop(Doer *root, Doer *workerA, Doer *workerB) {
    (void)root; // 现在还没用到 root，将来扩展

    Doer *all_doers[] = { workerA, workerB };
    Scheduler sched;
    scheduler_init(&sched, all_doers, 2);

    printf("=== CMD v3 Unified Loop ===\n");
    printf("指令形式：\n");
    printf("  a hello      → 仅 workerA 处理\n");
    printf("  b hi         → 仅 workerB 处理\n");
    printf("  both yo      → A/B 都处理\n");
    printf("  （无前缀）   → A/B 轮流处理\n");
    printf("  exit         → 退出\n\n");

    char buf[256];

    for (;;) {
        printf(">> ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\n[root] EOF, exit\n");
            break;
        }
        buf[strcspn(buf, "\n")] = 0; // 去掉换行

        // 1. root 把输入解析成消息，塞到某些 doer 的 inbox
        bool cont = parse_and_dispatch_input(buf, workerA, workerB);
        // cont = false → 用户输入 exit
        // 但我们仍然调度一步，让 worker 处理 EXIT

        // 2. scheduler 跑若干步，直到当前所有 inbox 暂时空了
        while (scheduler_step(&sched)) {
            // 不做其他事：单纯把队列清空
        }

        if (!cont) {
            printf("=== CMD System Exit ===\n");
            break;
        }
    }
}

/* ===== main ===== */

int main(void) {
    Doer root, workerA, workerB;

    doer_init(&root,   "root",    1);
    doer_init(&workerA,"workerA", 2);
    doer_init(&workerB,"workerB", 3);

    run_loop(&root, &workerA, &workerB);
    return 0;
}
