## MODIFIED Requirements

### Requirement: Holiday rule taxonomy
The rule engine SHALL support exactly these rule kinds: fixed date `(month, day)`; fixed date on a specific weekday `(month, day, weekday)` which is emitted only when that calendar date falls on `weekday` (no observance shift is applied); nth weekday of month `(n, weekday, month)` where `n < 0` counts from the end of the month (last weekday = `-1`); Easter-relative `(offsetDays)` measured from Easter Sunday; and one-off special date `(date)`. Each rule SHALL accept an optional validity window (`fromYear`, `toYear`) and an observance policy, with a calendar-level default observance policy.

#### Scenario: Nth weekday from start
- **WHEN** a rule is `NthWeekdayOfMonth(3, Monday, January)` for year 2025
- **THEN** it resolves to 2025-01-20 (third Monday of January)

#### Scenario: Nth weekday from end
- **WHEN** a rule is `NthWeekdayOfMonth(-1, Monday, May)` for year 2025
- **THEN** it resolves to 2025-05-26 (last Monday of May)

#### Scenario: Easter-relative rule
- **WHEN** a rule is `EasterRelative(-2)` (Good Friday) for year 2025
- **THEN** it resolves to 2025-04-18 (Easter Sunday 2025-04-20 minus 2 days)

#### Scenario: Validity window excludes years before fromYear
- **WHEN** a rule has `fromYear = 2021` (e.g. Juneteenth, June 19)
- **THEN** the holiday is generated for 2021 and later but NOT for 2020 or earlier

#### Scenario: Fixed-on-weekday rule fires only on the target weekday
- **WHEN** a rule is `FixedOnWeekday(September, 17, Friday)`
- **THEN** it resolves to September 17 in years where September 17 is a Friday, and resolves to nothing in years where it is any other weekday

### Requirement: CLSA includes solstice-based and one-off national holidays
The CLSA calendar SHALL include the *Día Nacional de los Pueblos Indígenas* (Ley 21.357, from 2021), whose date tracks the June solstice, and the genuinely one-time national holidays declared by special laws that are NOT covered by a permanent rule (e.g. the 2010 bicentenario days, the 2017 census day, the 2022 plebiscite day, and the historical Fiestas Patrias bridge days declared by single-year laws). Because these are not expressible as recurring rules, they MAY be represented as a maintained date table. The table SHALL include only genuine historical instances (up to and including 2025) and SHALL NOT project speculative future bridge days that depend on not-yet-enacted laws; permanent statutory bridges (Ley 20.983) SHALL be expressed as rules instead.

#### Scenario: Indigenous Peoples' Day present with solstice date
- **WHEN** building CLSA covering 2021 and 2024
- **THEN** the calendar includes Jun 21 2021 and Jun 20 2024 for Día Nacional de los Pueblos Indígenas

#### Scenario: Genuine historical one-off present
- **WHEN** building CLSA covering 2022
- **THEN** the calendar includes the Sep 16 2022 one-off national holiday

#### Scenario: No speculative future bridge projected
- **WHEN** building CLSA covering a year ≥ 2026 whose only candidate bridge is a non-statutory Sep-17-Monday or Sep-20-Friday case
- **THEN** that bridge day is NOT added (only permanent statutory holidays and rules apply)

## ADDED Requirements

### Requirement: CLSA models Ley 20.983 Fiestas Patrias and New-Year bridges
The CLSA calendar SHALL implement Ley 20.983 (effective 2017) as rules: September 17 SHALL be a holiday in any year (≥ 2017) where September 18 falls on a Saturday and September 19 on a Sunday — equivalently, where September 17 falls on a Friday; and January 2 SHALL be a holiday in any year (≥ 2017) where January 1 falls on a Sunday — equivalently, where January 2 falls on a Monday. These holidays SHALL NOT be date-shifted. Because CLBA reuses the CLSA ruleset, CLBA SHALL inherit both rules.

#### Scenario: Fiestas Patrias bridge applies (Sep 17)
- **WHEN** building CLSA for 2021, where Sep 18 is a Saturday and Sep 19 a Sunday
- **THEN** the calendar includes September 17 2021 (a Friday)

#### Scenario: New-Year bridge applies (Jan 2)
- **WHEN** building CLSA for 2023, where Jan 1 is a Sunday
- **THEN** the calendar includes January 2 2023 (a Monday)

#### Scenario: Bridges do not apply when conditions are not met
- **WHEN** building CLSA for 2025 (Sep 17 is not a Friday) and 2024 (Jan 1 is not a Sunday)
- **THEN** the calendar does NOT include September 17 2025 and does NOT include January 2 2024

#### Scenario: CLBA inherits the Ley 20.983 rules
- **WHEN** building CLBA for 2021 and 2023
- **THEN** the calendar includes September 17 2021 and January 2 2023, in addition to the December 31 banking holiday

#### Scenario: Rules are inactive before 2017
- **WHEN** building CLSA for a pre-2017 year where Sep 17 falls on a Friday
- **THEN** the Ley 20.983 rule does NOT generate September 17 for that year (the law was not yet in force)
