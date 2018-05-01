# QC Derivatives Valuation Engine

Clases y funciones para la valorización de swaps, cálculo de sensibilidad y construcción de curvas cupón cero.

El proyecto tiene cuatro componentes:

- QCDVECORE: este es el core de la solución. Todas las clases y funciones que valorizan, sensibilizan y construyen curvas son parte de este proyecto.
- QCDVEPYBIND: binding hacia Python diseñado para la integración con la aplicación Front Desk.
- QC_Financial: binding hacia Python totalmente genérico que permite el uso del core desde cualquier aplicación escrita en Python.
- QCDVEXLL: binding hacia Excel de parte de la funcionalidad del core. Esta pieza depende de XLW 5.0 (Mark Joshi).

El proyecto está aún en pleno desarrollo y cualquiera de sus componentes puede sufrir cambios y romper APIs.