## MODIFIED Requirements

### Requirement: CLSA includes solstice-based and one-off national holidays
The CLSA calendar SHALL include the *Día Nacional de los Pueblos Indígenas* (Ley 21.357, from 2021), whose date tracks the June solstice, and the genuinely one-time national holidays declared by special laws that are NOT covered by any permanent rule (the 2010 bicentenario days, the 2017 census day, and the 2022 plebiscite day). Because these are not expressible as recurring rules, they MAY be represented as a maintained date table. Permanent statutory bridges (Ley 20.983 and Ley 20.215 Article 35 ter) SHALL be expressed as rules instead, NOT as table entries.

#### Scenario: Indigenous Peoples' Day present with solstice date
- **WHEN** building CLSA covering 2021 and 2024
- **THEN** the calendar includes Jun 21 2021 and Jun 20 2024 for Día Nacional de los Pueblos Indígenas

#### Scenario: Genuine historical one-off present
- **WHEN** building CLSA covering 2022
- **THEN** the calendar includes the Sep 16 2022 one-off national holiday

#### Scenario: Statutory bridges are not table entries
- **WHEN** building CLSA covering any year
- **THEN** the Sep-17-Monday and Sep-20-Friday bridges come from the Ley 20.215 Article 35 ter rules (not the one-off table), so they apply uniformly to all qualifying years ≥ 2007 rather than only to a fixed historical set

## ADDED Requirements

### Requirement: CLSA models Ley 20.215 Article 35 ter Fiestas Patrias bridges
The CLSA calendar SHALL implement Ley 20.215 Article 35 ter (effective 2007) as rules: September 17 SHALL be a holiday in any year (≥ 2007) where September 18 falls on a Tuesday and September 19 on a Wednesday — equivalently, where September 17 falls on a Monday; and September 20 SHALL be a holiday in any year (≥ 2007) where September 18 falls on a Wednesday and September 19 on a Thursday — equivalently, where September 20 falls on a Friday. These holidays SHALL NOT be date-shifted. Because CLBA reuses the CLSA ruleset, CLBA SHALL inherit both rules.

#### Scenario: Sep 17 Monday bridge applies
- **WHEN** building CLSA for 2029, where Sep 18 is a Tuesday and Sep 19 a Wednesday
- **THEN** the calendar includes September 17 2029 (a Monday)

#### Scenario: Sep 20 Friday bridge applies
- **WHEN** building CLSA for 2030, where Sep 18 is a Wednesday and Sep 19 a Thursday
- **THEN** the calendar includes September 20 2030 (a Friday)

#### Scenario: Bridges do not apply when conditions are not met
- **WHEN** building CLSA for 2025, where Sep 17 is not a Monday and Sep 20 is not a Friday
- **THEN** the calendar includes neither September 17 2025 nor September 20 2025

#### Scenario: Rules are inactive before 2007
- **WHEN** building CLSA for 2001, where Sep 17 falls on a Monday
- **THEN** the Article 35 ter rule does NOT generate September 17 for that year (the law was not yet in force)

#### Scenario: CLBA inherits the Article 35 ter rules
- **WHEN** building CLBA for 2029
- **THEN** the calendar includes September 17 2029, in addition to the December 31 banking holiday
