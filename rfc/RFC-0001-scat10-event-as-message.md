# RFC-0001: SCAT10 — Event as Message

## Status

Accepted

## Context

CMR defines a system boundary where all external inputs
must enter the system as explicit Messages.

Prior to SCAT10, message handling in CMR had been validated
for internal application-generated messages only.
External events (such as stdin input) had not yet been
modeled under the same semantic rules.

This RFC records the first validation that
**external events can and must be represented as Messages**
within CMR.

## Decision

All external events entering CMR MUST be converted into
Messages before interacting with runtime execution.

An event is not processed directly.
It is first:
1. captured at the boundary,
2. normalized into a Message,
3. routed through `runtime_route`,
4. validated by capability rules,
5. executed by a Doer.

No special execution path exists for events.

## Implementation

SCAT10 introduces a boundary adapter that converts
stdin input into a Message of kind `MSGK_STDIN_LINE`.

The adapter:
- trims input,
- discards empty events,
- assigns an explicit target,
- assigns an explicit capability,
- routes the Message through the normal runtime path.

Once converted, the event is indistinguishable
from any other Message in the system.

## Observations

- Events obey the same capability validation rules as messages.
- Events can be accepted, dropped, or handled with full accounting.
- No implicit permission is granted due to origin.
- Message balance invariants remain satisfied.

This confirms that CMR can absorb real-world input
without violating semantic closure.

## Consequences

After SCAT10:
- There is no concept of a "raw event" inside CMR.
- External reality interacts with CMR only through Messages.
- System responsibility remains fully explicit and observable.

This establishes the minimal boundary model required
for integrating future I/O systems (e.g. epoll, io_uring)
under the same architectural rules.

## Reference Implementation

A reference implementation is provided at:

`rfc/implementations/scat10/`

This implementation is normative for RFC-0001.

## Related Work

- SCAT series roadmap
- CMR axioms v0.1

## Summary

SCAT10 validates that:
> **Events are messages, or they do not exist in CMR.**

This RFC marks the first successful integration
of external input into CMR without exception paths.
