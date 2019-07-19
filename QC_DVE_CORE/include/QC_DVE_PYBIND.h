#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef	_DEBUG
#endif

#include <Python.h>
#include <string>
#include <iostream>

#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif

/*
1.11. Writing Extensions in C++
It is possible to write extension modules in C++. Some restrictions apply. If the
main program (the Python interpreter) is compiled and linked by the C compiler,
global or static objects with constructors cannot be used. This is not a
problem if the main program is linked by the C++ compiler.
Functions that will be called by the Python interpreter (in particular,
module initialization functions) have to be declared using extern "C". It is
unnecessary to enclose the Python header files in extern "C" {...} they use
this form already if the symbol __cplusplus is defined (all recent C++ compilers
define this symbol).
*/