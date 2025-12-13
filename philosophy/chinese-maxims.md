# CMT 三条元训（Chinese Maxims of the CMT Axioms）

> 古之治军，以令为本；  
> 今之治算，以能为基。  
> 令有所属，事有所归，  
> 此 CMT 宇宙之三纲也。

本文件以中国古典式语言，表达 CMT（Capability–Message–Task）  
之三条根本公理，作为系统哲学之核心简明形式。


---

# 一、无令不行  
_No Action Without Capability_

**古义：**  
凡欲行事，先有其令。无令而动，则为乱；无权而为，则为僭。  
令者，事之始也；能者，行之基也。

**技术语义：**  
- Capability 是 **唯一** 的权限来源。  
- Task 不得在未获显式 Capability 之情况下执行任何操作。  
- Ambient authority（隐式权限）在 CMT 中不存在。  
- 任何可观察之行为均需可追溯至某个 Capability。

**对应公理：**  
> Authority derives only from capabilities.


---

# 二、行必有令  
_All Action Must Be Authorized Explicitly_

**古义：**  
既行其事，必明其令。令之所至，事之所起；  
令不至，则事不得起。  
传令必当昭然，不可潜行。

**技术语义：**  
- Capability 的转移必须依靠 **显式 Message**。  
- 不得隐式继承、不得外泄、不得凭空生成。  
- Message 是指派与授权的唯一通道。  
- 授权链条应完整、可见、可验证。

**对应公理：**  
> Capabilities transfer only via explicit messages.


---

# 三、功成返令  
_Completion Requires Message Response and Capability Return_

**古义：**  
功既成，当返其令；  
令返，则势收；事毕，则界清。  
明始有终，天下乃治。

**技术语义：**  
- Task 必须在执行完成后，以 Message 回复其结果。  
- 所获 Capability 必须显式归还、撤销或转交。  
- 不得遗留悬挂权限（dangling authority）。  
- 系统行为通过 Message 闭环收敛，保持可证性与稳态。

**对应公理：**  
> All communication and state effects occur via messages, forming a closed and auditable loop.


---

# 总旨（Grand Principle）

> **令明则能清，  
> 能清则事直，  
> 事直则算明，  
> 算明则天下其治。**

现代语即：

- 权限若明，则能力可控；  
- 能力可控，则任务简洁；  
- 任务简洁，则系统可证；  
- 系统可证，则运行可治。

此即 CMT 宇宙之本体论精神。


---

# 结构对照（Mapping Table）

| 元训（古文） | 技术术语 | 对应基元 | 对应公理 |
|--------------|----------|-----------|------------|
| 无令不行 | 权限来源必须显式 | Capability | 公理 1 |
| 行必有令 | 授权必须通过消息传递 | Message + Capability | 公理 2 |
| 功成返令 | 行为闭环 + 权限回收 | Task + Message | 公理 3 |

---

# 附：为何 CMT 天然符合古典治理思想？

古代军政体系依：

- 令（权能）  
- 信（消息）  
- 将（执行者）  

为基本运行单元，与 CMT 三基元完全同构。

故以古汉语表达 CMT 公理，  
不仅简洁优雅，更能揭示其本体结构之自然性与历史深度。

---

（完）
