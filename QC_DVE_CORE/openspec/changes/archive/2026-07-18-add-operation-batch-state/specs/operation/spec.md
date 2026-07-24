# operation

## ADDED Requirements

### Requirement: Operation wraps legs under an opaque key
An `Operation` SHALL be constructible from one or more existing `Leg` objects (as produced by `LegFactory` builders), a caller-supplied opaque `int64` key, and one `RecPay` direction per leg. qcfinancial SHALL attach no meaning to the key. Legs SHALL be identified within the operation by `leg_number` starting at 1 in construction order.

#### Scenario: Single-leg fixed-rate operation
- **WHEN** an `Operation` is built with key `1001` and one fixed-rate leg from `LegFactory::buildBulletFixedRateLeg` (or the custom-amortization builder)
- **THEN** the operation reports key `1001`, one leg with `leg_number` 1, and exposes the same cashflows (same `show`/`record` content) as the leg it was built from

#### Scenario: Multi-leg operation
- **WHEN** an `Operation` is built with two legs and directions `[RECEIVE, PAY]`
- **THEN** the legs are numbered 1 and 2 in construction order and each reports its supplied direction

#### Scenario: Empty operation rejected
- **WHEN** an `Operation` is constructed with zero legs, or with a `rec_pay` list whose length differs from the number of legs
- **THEN** construction fails with an error surfaced as a Python `ValueError`

### Requirement: Operation is immutable
An `Operation` SHALL expose no mutating methods after construction. Restructurings SHALL be modeled by the caller as new Operations.

#### Scenario: No mutation surface
- **WHEN** the Python API of `Operation` is inspected
- **THEN** it exposes only accessors (key, number of legs, legs, directions) and no method that alters legs, key, or directions
