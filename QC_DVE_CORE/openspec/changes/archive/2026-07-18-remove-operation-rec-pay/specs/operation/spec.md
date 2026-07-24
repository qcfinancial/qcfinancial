# operation

## MODIFIED Requirements

### Requirement: Operation wraps legs under an opaque key
An `Operation` SHALL be constructible from one or more existing `Leg` objects (as produced by `LegFactory` builders) and a caller-supplied opaque `int64` key. qcfinancial SHALL attach no meaning to the key. Legs SHALL be identified within the operation by `leg_number` starting at 1 in construction order. An `Operation` SHALL NOT carry a per-leg direction: receive/pay is already expressed by the signs of the cashflow amounts, which `LegFactory` fixes at leg construction.

#### Scenario: Single-leg fixed-rate operation
- **WHEN** an `Operation` is built with key `1001` and one fixed-rate leg from `LegFactory::buildBulletFixedRateLeg` (or the custom-amortization builder)
- **THEN** the operation reports key `1001`, one leg with `leg_number` 1, and exposes the same cashflows (same `show`/`record` content) as the leg it was built from

#### Scenario: Multi-leg operation
- **WHEN** an `Operation` is built with two legs, the first built with `RECEIVE` and the second with `PAY`
- **THEN** the legs are numbered 1 and 2 in construction order, and the direction of each remains readable from the signs of its cashflow amounts

#### Scenario: Empty operation rejected
- **WHEN** an `Operation` is constructed with zero legs, or with a leg containing zero cashflows
- **THEN** construction fails with an error surfaced as a Python `ValueError`

### Requirement: Operation is immutable
An `Operation` SHALL expose no mutating methods after construction. Restructurings SHALL be modeled by the caller as new Operations.

#### Scenario: No mutation surface
- **WHEN** the Python API of `Operation` is inspected
- **THEN** it exposes only accessors (key, number of legs, legs) and no method that alters legs or key

