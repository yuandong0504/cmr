// multi_doer_v2.c
// 单线程 unified event loop：root-doer 输入 + worker-doer 调度

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
    MSG_PRINT,
    MSG_EXIT,
    MSG_NONE
} MessageType;

typedef struct {
    MessageType type;
    char payload[128];
} Message;

/* ===== Simple queue ===== */
#define QMAX 64
typedef struct {
    Message q[QMAX];
    int head, tail;
} MsgQueue;

static void q_init(MsgQueue *mq) { mq->head = mq->tail = 0; }
static bool q_empty(MsgQueue *mq) { return mq->tail == mq->head; }

static bool q_push(MsgQueue *mq, Message *m) {
    int next = (mq->tail + 1) % QMAX;
    if (next == mq->head) return false;
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
typedef struct {
    const char *name;
    Capability cap;
    MsgQueue inbox;
} Doer;

static void doer_handle(Doer *d, Message *msg) {
    if (msg->type == MSG_PRINT) {
        printf("[Doer %s / cap=%d] %s\n",
               d->name, d->cap.id, msg->payload);
    }
}

/* ===== Parse root input ===== */
static Message parse_input(const char *line) {
    Message m;

    if (strcmp(line, "exit") == 0) {
        m.type = MSG_EXIT;
        m.payload[0] = '\0';
        return m;
    }

    m.type = MSG_PRINT;
    strncpy(m.payload, line, sizeof(m.payload));
    m.payload[sizeof(m.payload)-1] = '\0';
    return m;
}

/* ===== Unified event loop ===== */
static void run_loop(Doer *root, Doer *wA, Doer *wB) {
    printf("=== CMD Unified Loop Started ===\n");
    printf("输入文本 → workerA/B 会处理\n输入 exit → 终止\n\n");

    char buf[128];
    int turn = 0;

    for (;;) {

        /* ===== 1. root-doer 输入（非阻塞模拟） ===== */
        printf(">> ");
        fflush(stdout);

        if (fgets(buf, sizeof(buf), stdin)) {
            buf[strcspn(buf, "\n")] = 0;

            Message msg = parse_input(buf);

            if (msg.type == MSG_EXIT) {
                q_push(&wA->inbox, &msg);
                q_push(&wB->inbox, &msg);
                break;
            }

            if (turn == 0) q_push(&wA->inbox, &msg);
            else           q_push(&wB->inbox, &msg);

            turn ^= 1;
        }

        /* ===== 2. scheduler 扫描 worker ===== */
        Message tmp;

        if (q_pop(&wA->inbox, &tmp)) doer_handle(wA, &tmp);
        if (q_pop(&wB->inbox, &tmp)) doer_handle(wB, &tmp);
    }

    printf("=== CMD System Exit ===\n");
}

/* ===== main ===== */
int main() {
    Doer root = { .name="root", .cap={1} };
    Doer wA   = { .name="workerA", .cap={2} };
    Doer wB   = { .name="workerB", .cap={3} };

    q_init(&root.inbox);
    q_init(&wA.inbox);
    q_init(&wB.inbox);

    run_loop(&root, &wA, &wB);
    return 0;
}
