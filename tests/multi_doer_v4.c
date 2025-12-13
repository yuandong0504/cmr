// multi_doer_v4.c
// CMD v4: Doer → Doer Direct Messaging
// gcc -std=c11 -Wall -Wextra -O2 multi_doer_v4.c -o multi_doer_v4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ===== Capability ===== */
typedef struct {
    int id;
} Capability;

/* ===== Message ===== */
typedef struct {
    char from[32];      // 谁发来的
    char payload[128];  // 内容
} Message;

/* ===== Simple Queue ===== */
#define QMAX 64

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

/* ===== Doer ===== */
typedef struct Doer {
    const char *name;
    Capability cap;
    MsgQueue inbox;

    void (*handle)(struct Doer *, const Message *);
} Doer;

/* ===== Doer handler ===== */
static void doer_handle(Doer *d, const Message *m) {
    printf("[Doer %s / cap=%d] from %s: %s\n",
           d->name, d->cap.id, m->from, m->payload);
}

/* Initialize doer */
static void doer_init(Doer *d, const char *name, int cap_id) {
    d->name = name;
    d->cap.id = cap_id;
    q_init(&d->inbox);
    d->handle = doer_handle;  // 注入行为
}

/* ===== Map name ("A","B") → Doer pointer ===== */
static Doer* name_to_doer(const char *name, Doer *A, Doer *B) {
    if (strcmp(name, "A") == 0) return A;
    if (strcmp(name, "B") == 0) return B;
    return NULL;
}

/* ===== Scheduler: drain all inboxes ===== */
static void scheduler_step(Doer **doers, int n) {
    Message m;

    for (int i = 0; i < n; i++) {
        while (q_pop(&doers[i]->inbox, &m)) {
            doers[i]->handle(doers[i], &m);
        }
    }
}

/* ===== Parse root commands ===== */
static bool parse_and_dispatch(
    const char *line,
    Doer *root, Doer *A, Doer *B)
{
    // ---- exit ----
    if (strcmp(line, "exit") == 0) {
        Message ex = {0};
        strcpy(ex.from, "root");
        strcpy(ex.payload, "EXIT");

        q_push(&A->inbox, &ex);
        q_push(&B->inbox, &ex);
        return false; // quit
    }

    // ---- send A B msg ----
    if (strncmp(line, "send ", 5) == 0) {
        char src[32], dst[32], msg[128];

        if (sscanf(line + 5, "%31s %31s %127[^\n]", src, dst, msg) == 3) {
            Doer *fromD = name_to_doer(src, A, B);
            Doer *toD   = name_to_doer(dst, A, B);

            if (fromD && toD) {
                Message m = {0};
                strcpy(m.from, fromD->name);
                strcpy(m.payload, msg);
                q_push(&toD->inbox, &m);
                return true;
            }
        }
    }

    // ---- no prefix → broadcast ----
    Message m = {0};
    strcpy(m.from, "root");
    strcpy(m.payload, line);

    q_push(&A->inbox, &m);
    q_push(&B->inbox, &m);

    return true;
}

/* ===== Main unified loop ===== */
int main(void) {
    Doer root, A, B;

    doer_init(&root, "root",    1);
    doer_init(&A,    "workerA", 2);
    doer_init(&B,    "workerB", 3);

    printf("=== CMD v4: Doer → Doer Direct Messaging ===\n");
    printf("指令格式:\n");
    printf("  send A B hello tony    → A 发给 B\n");
    printf("  send B A hi 大哥       → B 发给 A\n");
    printf("  (无前缀) text          → root 广播到 A/B\n");
    printf("  exit                   → 退出\n\n");

    char buf[256];

    for (;;) {
        printf(">> ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) break;
        buf[strcspn(buf, "\n")] = 0;

        bool cont = parse_and_dispatch(buf, &root, &A, &B);

        Doer *all[] = {&A, &B};
        scheduler_step(all, 2);

        if (!cont) break;
    }

    printf("=== CMD system exited ===\n");
    return 0;
}
