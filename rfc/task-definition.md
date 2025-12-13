# Task：计算主体的本体论（v1.0）
CMT Runtime — The Ontology of the Computational Task

本文件定义 CMT 宇宙中的核心基元之一：**Task**。  
Task 是计算的主体（agent），是能力的承载者，是消息的解释者，  
并且是所有行为的唯一发生点。

本定义为 CMT 运行时的正式哲学基础、语义基础与规范基础。

---

# 1. Task 的本体地位（What a Task *is*）

**任务（Task）是计算中的最小主体单位（Minimal Computational Agent）。**

一个 Task 必须具备以下四个存在条件：

1. **私域状态（Private State）**  
   Task 内部拥有不可被直接访问的局部状态。

2. **能力集合（Capability Set）**  
   Task 能执行的所有动作，完全由其持有的 Capability 决定。

3. **消息信箱（Message Inbox）**  
   Task 的唯一输入通道；所有交互都由消息进入。

4. **反应式执行循环（Reactive Execution Loop）**  
   Task 通过处理消息来推进其行为。

> **Task = State + Capabilities + Inbox + Reactive Loop**

Task 不是代码片段、不是线程、不是协程、不是 Actor、不是进程。  
它代表 **计算中的“一个个体”**。

---

# 2. Task 与传统概念的根本区别

| 概念 | 为什么不是 Task？ |
|------|--------------------|
| Thread | 共享内存，无边界；可被任意抢占；权限混乱 |
| Process | 过于粗；无清晰生命周期语义；非主体而是容器 |
| Coroutine | 语法糖；缺权限；缺消息入口 |
| Actor | 无 capability；缺闭环；权限不可验证 |
| Future/Promise | 状态载体，不是主体 |
| OS Handler | 机制，不是个体 |

**Task 是第一次将“执行者”升为一等公民（first-class agent）。**

---

# 3. Task 的运行边界（Boundary of a Task）

一个 Task 的边界由三部分组成：

1. **Memory boundary（私域内存）**  
   - 只能被 Task 自己访问  
   - 不可被其他 Task 读取或写入  
   - 形成天然并发安全

2. **Authority boundary（能力边界）**  
   - Task 能做的所有事，必须来自其能力集合  
   - 没有能力 = 无法产生行为

3. **Communication boundary（消息边界）**  
   - Task 之间不能共享内存  
   - 唯一交互方式：Message  
   - 无任何隐式通道

这三个边界构成 Task 的宇宙位置：

> **Task 是内收的存在；  
其行为通过消息展开；  
其权能由 Capability 限定。**

---

# 4. Task 的生命周期（Lifecycle of a Task）

Task 的生命周期是一个可审计、可推理、可验证的闭环：
