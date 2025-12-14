# CMR — Capability Message Runtime

CMR is a runtime architecture built on one non-negotiable principle:

> **A system that achieves semantic closure  
> has fulfilled all of its responsibilities as a system.**

---
> This document defines the non-negotiable axioms of CMR.
> All implementations MUST conform to these principles.
> Any deviation indicates a different system.

## Core Axioms

CMR is governed by the following axioms.  
They are not guidelines, but invariants.

### 1. Semantic Closure First

Every input to the system **must** reach a well-defined semantic outcome.

There are no:
- half-processed inputs
- silent drops
- implicit human fallbacks
- undefined states

If an input enters the system, the system must be able to explain:
- what happened
- why it happened
- whether it was accepted, rejected, dispatched, or terminated

---

### 2. Rules Before Actions

All behavior is a **consequence of rules**, never the other way around.

- Rules define what actions are possible
- Actions never rewrite or bypass rules
- No behavior is justified retroactively

System evolution happens by modifying rules, not by patching behavior.

---

### 3. No Supervisor

CMR explicitly forbids runtime entities with arbitrary override power.

There is no component that can:
- bypass rules
- override dispatch decisions
- intervene without leaving a semantic trace

Supervisor-style “human-in-the-loop” control is treated as a design failure,
not a safety mechanism.

---

### 4. Leader Only

Authority exists **only at design time**, not at runtime.

Leaders:
- define rules
- define capabilities
- define message semantics

They do **not**:
- intervene in execution
- override runtime decisions
- handle exceptions ad hoc

Runtime executes rules.  
Leadership ends before execution begins.

---

### 5. No Tick Dependency

CMR does not rely on ticks, heartbeats, or periodic polling
to drive system progress.

- Progress is caused by messages, not by time slices
- No component advances "because time passed"
- Idle systems remain idle without artificial activity

Tick-based execution is a consequence of thread scheduling,
not a fundamental property of computation.

CMT does not require ticks to remain live, responsive, or correct.

## Responsibility Boundary

CMR makes a strict distinction between **system responsibility** and **world responsibility**.

The system does **not** promise:
- success
- fairness
- optimal outcomes
- real-world satisfaction

The system **does** promise:
- semantic completeness
- explicit decisions
- observable outcomes
- zero hidden discretion

Once semantic closure is achieved, the system is complete.

---

## Design Position

CMR chooses to make complexity explicit rather than hidden.

It prefers:
- clear rejection over silent acceptance
- explicit failure over ambiguous success
- rule changes over behavioral exceptions

This makes the system harder to design,
but easier to trust.

---
## Status

This document defines CMR axioms v0.1.
Future revisions may clarify wording,
but MUST NOT weaken or contradict these axioms.

## Summary

**Semantic Closure First.**  
**Rules before Actions.**  
**No Supervisor.**  
**Leader only.**

A system that achieves semantic closure  
has fulfilled all of its responsibilities as a system.


