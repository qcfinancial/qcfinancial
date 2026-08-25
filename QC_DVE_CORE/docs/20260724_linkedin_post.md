# LinkedIn post — qcfinancial 1.13.0

## English version

**qcfinancial 1.13.0 is out — and this release is all about speed.**

We rebuilt how business-day adjustment works under the hood: leg construction across a realistic holiday calendar is now up to **180x faster**, with output verified byte-identical across hundreds of test cases. If you're pricing large portfolios, this is the kind of change that turns minutes into milliseconds.

This release also introduces **Operation** and **Portfolio**, a new batch layer for querying portfolio state at scale — accrued interest, outstanding notional, upcoming flows, and present value, returned as columnar numpy arrays and computed in parallel across positions.

Alongside that, we tightened up precision in our overnight-rate cashflow calculations (compounded and index-based), so realized coupons line up exactly with what settles.

qcfinancial keeps getting faster and more precise, release after release.

#quantfinance #fintech #opensource #interestrates #python #cpp

## Versión en español

**Ya está disponible qcfinancial 1.13.0 — y esta versión es pura velocidad.**

Rediseñamos por dentro el ajuste de días hábiles: la construcción de patas de flujo sobre un calendario de feriados realista ahora es hasta **180 veces más rápida**, con resultados verificados como idénticos, byte a byte, en cientos de casos de prueba. Si valorizas portafolios grandes, este es el tipo de cambio que convierte minutos en milisegundos.

Esta versión también incorpora **Operation** y **Portfolio**, una nueva capa de consultas por lotes para obtener el estado de un portafolio a escala — interés devengado, nocional vigente, próximos flujos y valor presente, entregados como arreglos numpy columnares y calculados en paralelo entre posiciones.

Además, afinamos la precisión en el cálculo de flujos de tasa overnight (compuestas e indexadas), para que los cupones realizados calcen exactamente con lo que se liquida.

qcfinancial sigue siendo más rápido y más preciso, versión tras versión.

#quantfinance #fintech #opensource #tasasdeinteres #python #cpp
