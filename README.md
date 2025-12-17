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

---

### 1. Semantic Closure First

Every input to the system **MUST** reach a well-defined semantic outcome.

There are no:
- half-processed inputs
- silent drops
- implicit human fallbacks
- undefined states

If an input enters the system, the system **MUST** be able to explain:
- what happened
- why it happened
- whether it was accepted, rejected, dispatched, or terminated

If the system cannot explain an outcome,  
the system has failed its primary responsibility.

---

### 2. Rules Before Actions

All behavior is a **consequence of rules**, never the reverse.

- Rules define what actions are possible
- Actions MUST NOT rewrite, bypass, or reinterpret rules
- No behavior is justified retroactively

System evolution happens by modifying rules,  
not by patching behavior.

Behavior without rule origin is considered invalid.

---

### 3. Explicit Authority Only

All authority in CMR is **explicit**, never inferred.

- Authority is minted, not guessed
- Authority is validated, not assumed
- Authority is executed, not extended
- Authority is accounted for, not hidden

There is no implicit permission, contextual inference,
or heuristic escalation.

If authority is not explicit, it does not exist.

---

### 4. No Supervisor

CMR explicitly forbids runtime entities with arbitrary override power.

There is no component that may:
- bypass rules
- override dispatch decisions
- intervene without leaving a semantic trace

Supervisor-style “human-in-the-loop” control is treated as a **design failure**,
not a safety mechanism.

Safety is achieved through explicit rules,
not discretionary intervention.

---

### 5. Leader Only (Design-Time Authority)

Authority exists **only at design time**, not at runtime.

Leaders:
- define rules
- define capabilities
- define message semantics

Leaders do **not**:
- intervene during execution
- override runtime decisions
- handle exceptions ad hoc

Leadership ends **before execution begins**.

Runtime executes rules.  
It does not reinterpret intent.

---

### 6. No Tick Dependency

CMR does not rely on ticks, heartbeats,
or periodic polling to drive system progress.

- Progress is driven by messages, not by time
- No component advances simply because time passed
- Idle systems remain idle without artificial activity

Tick-based execution is an artifact of thread scheduling,
not a fundamental property of computation.

CMR remains live, responsive, and correct
without time-driven forcing.

---

## Responsibility Boundary

CMR draws a strict boundary between **system responsibility**
and **world responsibility**.

The system does **NOT** promise:
- success
- fairness
- optimal outcomes
- real-world satisfaction

The system **DOES** promise:
- semantic completeness
- explicit decisions
- observable outcomes
- zero hidden discretion

Once semantic closure is achieved,
the system’s responsibility is complete.

---

## Design Position

CMR chooses to make complexity **explicit rather than hidden**.

It prefers:
- clear rejection over silent acceptance
- explicit failure over ambiguous success
- rule changes over behavioral exceptions

This makes the system harder to design,
but fundamentally easier to trust.

---

## Status

This document defines **CMR Axioms v0.1**.

Future revisions may clarify wording,
but **MUST NOT** weaken, bypass,
or contradict these axioms.

---

## Summary

**Semantic Closure First.**  
**Rules Before Actions.**  
**Explicit Authority Only.**  
**No Supervisor.**  
**Leader Only.**

> **A system that achieves semantic closure  
> has fulfilled all of its responsibilities as a system.**
