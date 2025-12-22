# Authority in CMR (Non-Negotiable)

This document defines the status, boundaries, and termination
of authority within the Capability Message Runtime (CMR).

This is not an implementation guide.
It is a declaration of invariants.

Any system that violates these constraints
is not a conforming implementation of CMR.

---

## Core Statement

**Authority does not exist at runtime in CMR.**

Authority may exist at design time
(in the form of rules, capability definitions, and message semantics),
but it MUST NOT exist during execution.

No runtime component is permitted to:
- invent authority
- infer authority
- override rules
- exercise discretionary power

---

## Design-Time Authority

Authority exists **only before execution begins**.

Design-time authority includes:
- defining capability types
- defining which capabilities are valid for which Doers
- defining message kinds and routing semantics
- defining closure invariants

Once execution starts, all authority is considered finalized.

Design-time authority **cannot intervene** during runtime.

---

## Runtime Components and Authority

### Mint

Mint creates **identities**, not permissions.

- Message IDs are identifiers, not authority.
- Capability IDs are labels, not authorization.

Mint MUST NOT:
- grant permissions
- imply legitimacy
- bypass validation

Mint has no authority.

---

### Validation

Validation is a pure rule check.

- It returns a boolean result.
- It has no side effects.
- It performs no recovery or escalation.

Validation does not decide outcomes.
It only answers whether a rule is satisfied.

Validation has no authority.

---

### Runtime

Runtime enforces semantic closure.

For every Message, runtime MUST produce
exactly one of the following outcomes:
- handled
- dropped
- pending (observable)

Runtime MUST NOT:
- reinterpret rules
- create exceptions
- apply discretionary judgment

Runtime executes rules.
It does not make them.

Runtime has no authority.

---

### Scheduler

The scheduler advances execution.

- It selects work that is already semantically closed.
- It does not determine legitimacy.
- It does not alter outcomes.

The scheduler is not a judge.
It is a pump.

The scheduler has no authority.

---

### Doers

Doers execute behavior defined by Messages.

- Doers cannot create new authority.
- Doers cannot override validation.
- Doers cannot escalate privileges.

A Doer may fail, reject, or drop work,
but only according to rules already defined.

Doers have no authority.

---

## Forbidden Constructs

The following constructs are explicitly forbidden in CMR:

- root users
- admin overrides
- supervisor processes
- emergency backdoors
- human-in-the-loop runtime intervention
- silent exception paths

Any mechanism that allows authority to re-enter
during runtime execution
violates CMR semantics.

---

## Rationale

Authority exercised during execution
cannot be audited, reasoned about, or bounded.

Such authority inevitably leads to:
- responsibility diffusion
- opaque failure modes
- non-reproducible behavior
- human dependency disguised as safety

CMR rejects these tradeoffs entirely.

---

## Implications

Because authority does not exist at runtime:

- All responsibility is explicit.
- All outcomes are observable.
- All failures are attributable.
- All behavior is reproducible.

This makes CMR harder to design,
but possible to trust.

---

## Final Declaration

**Authority ends where execution begins.**

Any system that requires runtime authority
to remain correct, safe, or humane
has already failed at the structural level.

CMR does not permit such systems.
