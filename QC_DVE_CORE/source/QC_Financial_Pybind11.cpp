//
// Created by Alvaro Diaz on 2019-07-15.
//

#include <pybind11.h>

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(QC_Financial_Pybind, m)
{
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &add, "A function which adds two numbers");
}
