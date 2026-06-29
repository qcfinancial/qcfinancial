## ADDED Requirements

### Requirement: Holiday-aware business-day adjustment
`nextBusinessDay` SHALL return the input date if it is a business day, otherwise the nearest following date that is neither a Saturday, a Sunday, nor a member of the holiday set. `previousBusinessDay` SHALL do the same searching backward. Holiday membership SHALL be tested using the holiday set's own logarithmic-time lookup (`std::set::find`/`count`), not a linear scan. The results SHALL be identical to the previous implementation.

#### Scenario: Weekday holiday is skipped forward
- **WHEN** the input date is a holiday that falls on a Wednesday
- **THEN** `nextBusinessDay` returns the following Thursday (assuming it is a business day)

#### Scenario: Friday holiday rolls to Monday
- **WHEN** the input date is a holiday that falls on a Friday
- **THEN** `nextBusinessDay` returns the following Monday (skipping the weekend)

#### Scenario: Monday holiday rolls back to Friday
- **WHEN** the input date is a holiday that falls on a Monday
- **THEN** `previousBusinessDay` returns the preceding Friday

#### Scenario: Consecutive holidays are all skipped
- **WHEN** several consecutive dates (including across a weekend) are all holidays
- **THEN** `nextBusinessDay` returns the first business day after the entire run

### Requirement: Calendar merge covers both calendars
`operator+` SHALL return a calendar whose holiday set is the union of the two operands' holidays, whose start date is the earlier of the two start dates, and whose length (in years) reaches at least the later of the two calendars' end years.

#### Scenario: Merged start date is the earlier of the two
- **WHEN** calendar A starts in 2023 and calendar B starts in 2020 and they are merged (in either order)
- **THEN** the merged calendar's start date is the 2020 date

#### Scenario: Merged length covers the later end
- **WHEN** calendar A spans 2020 for 5 years (through 2025) and calendar B spans 2023 for 3 years (through 2026) are merged
- **THEN** the merged calendar's start year is 2020 and its length reaches at least 2026

#### Scenario: Merged holidays are the union
- **WHEN** two calendars are merged
- **THEN** every holiday of either operand is present in the merged calendar, with duplicates collapsed
