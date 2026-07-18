# operation-portfolio

## ADDED Requirements

### Requirement: Portfolio holds operations with incremental mutation
A `Portfolio` SHALL hold Operations keyed by their opaque key, support `add(operation)` and `remove(key)` at any time between queries, and never require rebuilding inside a projection. It SHALL handle the target scale of 10⁵–10⁶ operations.

#### Scenario: Add and query
- **WHEN** an operation with key `1001` is added and a batch query runs
- **THEN** the query output contains rows for every leg of operation `1001`

#### Scenario: Remove and query
- **WHEN** operation `1001` is removed and a batch query runs
- **THEN** the output contains no row with `op_key == 1001` and all other operations are unaffected

#### Scenario: Duplicate add rejected
- **WHEN** an operation is added with a key already present
- **THEN** the add fails with a Python `ValueError` and the portfolio is unchanged

#### Scenario: Missing remove rejected
- **WHEN** `remove(key)` is called with a key not present
- **THEN** the call fails with a Python `ValueError` and the portfolio is unchanged

### Requirement: Deterministic row order
Batch query outputs SHALL order rows by `(op_key, leg_number)` ascending, independent of insertion order, removal history, and thread count.

#### Scenario: Insertion order does not matter
- **WHEN** the same set of operations is added in two different orders to two portfolios and the same query runs on both
- **THEN** both outputs are identical, ordered by `(op_key, leg_number)`
