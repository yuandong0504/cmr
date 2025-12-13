// doer_loop.c
// 最小可运行的 Doer 实现示例
// 编译：gcc -std=c11 -Wall -Wextra -O2 doer_loop.c -o doer_loop
// 运行：./doer_loop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ====== 基本常量 ====== */

#define MAX_LINE 256

/* ====== Capability：权力的最小单位 ====== */

typedef struct {
    int id;              // 这里只是示意：真实系统里会是不可伪造句柄
} Capability;

/* ====== Message：消息（命令载体） ====== */

typedef enum {
    MSG_PRINT,
    MSG_EXIT,
    MSG_UNKNOWN
} MessageType;

typedef struct {
    MessageType type;
    char payload[MAX_LINE];
} Message;

/* ====== Doer：行动主体（执行发生的点） ====== */

typedef struct {
    Capability cap;      // 这个 Doer 拥有哪些能力
    const char *name;    // 只是方便打印日志
} Doer;

/* ====== 简单的工具函数 ====== */

// 去掉行尾的 \n
static void trim_newline(char *s) {
    size_t n = strlen(s);
    if (n > 0 && s[n-1] == '\n') {
        s[n-1] = '\0';
    }
}

/* ====== 消息接收：从 stdin 收一条 Message ====== */

// 返回 true 表示成功收到一条消息，false 表示 EOF（可以退出 doer_loop）
static bool receive_message(Message *out) {
    char buf[MAX_LINE];

    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        // EOF or error
        return false;
    }

    trim_newline(buf);

    if (strcmp(buf, "exit") == 0 || strcmp(buf, "quit") == 0) {
        out->type = MSG_EXIT;
        out->payload[0] = '\0';
    } else if (buf[0] == '\0') {
        // 空行，算 unknown
        out->type = MSG_UNKNOWN;
        out->payload[0] = '\0';
    } else {
        out->type = MSG_PRINT;
        strncpy(out->payload, buf, sizeof(out->payload));
        out->payload[sizeof(out->payload) - 1] = '\0';
    }

    return true;
}

/* ====== Doer 具体行为：对一条 Message 的反应 ====== */

static void doer_handle_message(Doer *d, const Message *msg) {
    switch (msg->type) {
        case MSG_PRINT:
            // 这里模拟：Doer 拿着自己的 capability 来“说话”
            printf("[doer %s / cap=%d] says: %s\n",
                   d->name, d->cap.id, msg->payload);
            break;

        case MSG_EXIT:
            printf("[doer %s] received EXIT, will stop.\n", d->name);
            break;

        case MSG_UNKNOWN:
        default:
            printf("[doer %s] received unknown / empty message.\n", d->name);
            break;
    }
}

/* ====== Doer 主循环：doer_loop ====== */

static void doer_loop(Doer *d) {
    printf("[doer %s] started. Type something and press Enter.\n", d->name);
    printf("  - 普通文本：doer 会打印出来\n");
    printf("  - 输入 'exit' 或 'quit'：doer 停止\n\n");

    for (;;) {
        Message msg;

        printf(">> ");
        fflush(stdout);

        // receive_message 负责把 stdin 的一行变成一条 Message
        if (!receive_message(&msg)) {
            // EOF：例如 Ctrl+D
            printf("\n[doer %s] EOF, stopping.\n", d->name);
            break;
        }

        // 处理消息
        doer_handle_message(d, &msg);

        if (msg.type == MSG_EXIT) {
            break;
        }
    }

    printf("[doer %s] loop exited.\n", d->name);
}

/* ====== main：创建一个 root doer 并启动 ====== */

int main(void) {
    Doer root = {
        .cap = { .id = 1 },      // root doer 的初始 capability（示意）
        .name = "root-doer"
    };

    doer_loop(&root);

    return 0;
}
