# Instruments

All cashflow types implement the abstract `Cashflow` interface (`amount()`, `ccy()`, `date()`) and expose a `record()` method whose tuple ends with `present_value` and `discount_factor`.

## Active Cashflow Types

| Class | Python `show()` | Description |
|---|---|---|
| `SimpleCashflow` | ✅ | Single deterministic payment |
| `SimpleMultiCurrencyCashflow` | ✅ | Deterministic payment with FX conversion |
| `FixedRateCashflow` | ✅ | Fixed rate interest period |
| `FixedRateMultiCurrencyCashflow` | ✅ | Fixed rate with FX conversion |
| `IborCashflow` | ✅ | Floating IBOR-style period |
| `IborMultiCurrencyCashflow` | ✅ | IBOR with FX conversion |
| `IcpClpCashflow` | ✅ | Chilean overnight index (ICP) in CLP |
| `IcpClfCashflow` | ✅ | Chilean overnight index in CLF (UF) |
| `CompoundedOvernightRateCashflow2` | ✅ | Generic compounded overnight rate (SOFR, SONIA, …) |
| `CompoundedOvernightRateMultiCurrencyCashflow2` | ✅ | Compounded overnight with FX conversion |
| `OvernightIndexCashflow` | ✅ | Overnight index with flexible settlement |
| `OvernightIndexMultiCurrencyCashflow` | ✅ | Overnight index with FX conversion |

## LegFactory Builders

`LegFactory` provides static methods to construct full legs from schedule parameters.

| Method | Produces |
|---|---|
| `build_bullet_fixed_rate_leg` | `FixedRateCashflow` leg, bullet amortization |
| `build_custom_amort_fixed_rate_leg` | `FixedRateCashflow` leg, custom amortization |
| `build_bullet_fixed_rate_mccy_leg` | `FixedRateMultiCurrencyCashflow` leg, bullet |
| `build_custom_amort_fixed_rate_mccy_leg` | `FixedRateMultiCurrencyCashflow` leg, custom |
| `build_bullet_ibor_leg` | `IborCashflow` leg, bullet |
| `build_custom_amort_ibor_leg` | `IborCashflow` leg, custom |
| `build_bullet_ibor_mccy_leg` | `IborMultiCurrencyCashflow` leg, bullet |
| `build_custom_amort_ibor_mccy_leg` | `IborMultiCurrencyCashflow` leg, custom |
| `build_bullet_icp_clp_leg` | `IcpClpCashflow` leg, bullet |
| `build_custom_amort_icp_clp_leg` | `IcpClpCashflow` leg, custom |
| `build_bullet_icp_clf_leg` | `IcpClfCashflow` leg, bullet |
| `build_custom_amort_icp_clf_leg` | `IcpClfCashflow` leg, custom |
| `build_bullet_compounded_overnight_rate_leg_2` | `CompoundedOvernightRateCashflow2` leg, bullet |
| `build_custom_amort_compounded_overnight_rate_leg_2` | `CompoundedOvernightRateCashflow2` leg, custom |
| `build_bullet_overnight_index_leg` | `OvernightIndexCashflow` leg, bullet |
| `build_custom_amort_overnight_index_leg` | `OvernightIndexCashflow` leg, custom |

## Chilean Market Instruments

### ICP-CLP (`IcpClpCashflow`)

Implements the Chilean peso overnight index (ICP) compounding convention used in the local OIS market. The index value is set via `ForwardRates.set_rates_icp_clp_leg1`.

### ICP-CLF / UF (`IcpClfCashflow`)

Implements the UF-denominated overnight index convention. Forward rates are set via `ForwardRates.set_rates_icp_clf_leg`.

## Forward Rate Setting

Use `ForwardRates` to project rates onto floating legs before discounting:

```python
fwd = qf.ForwardRates()

# IBOR
fwd.set_rates_ibor_leg1(leg, projection_curve, valuation_date)

# Compounded overnight (SOFR, SONIA, …)
fwd.set_rates_compounded_overnight_leg2(leg, index_values, valuation_date)

# Overnight index
fwd.set_rates_overnight_index_leg(leg, index_values, valuation_date)

# ICP-CLF
fwd.set_rates_icp_clf_leg(leg, uf_series, valuation_date)
```
