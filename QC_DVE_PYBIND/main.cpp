#include "include.h"
#include "QCDate.h"
#include <string>

//PyObject define un objeto Python o utilizable por python
PyObject * some_function(PyObject * self, PyObject * args)
{
	int i;
	int j;


	//PyArg_ParseTuple obtiene los argumentos Python (argumentos que
	// le entregamos a la funcion desde python) y las tranforma a objetos C (son los valores alojados como referencia en la finción).
	//El retorno es TRUE si todos los argumentos estan bien identificados como 
	// objetos C
	//
	//i dice que la entrada es un int y si pongo ii impleca que la funcion tiene
	// dos argumentos y que los dos son int
	//
	//f: float
	//d: double
	//i: int
	//I: unsignedInt
	//c: char
	//s: (string or Unicode) [const char *]
	//S: (string)[PyStringObject *]
	//Extrañamente no encuentro BOOLEAN
	//|: el resto de los argumentos son opcionales
	//(items): tuple
	//[items]: list
	//{items}: dictionary
	//... Hay más https://docs.python.org/2/c-api/arg.html#c.PyArg_ParseTuple

	if (!PyArg_ParseTuple(args, "ii", &i, &j))
	{
		goto error;
	}

	//PyInt_FromLong transforma un valor C en un int Python
	//Py_BuildValue("i", i + j); que no es utilizado acá realiza la misma acción 
	// que la funcion anterior solo que en este caso es mas generica ya 
	// que utiliza los mismo identificadores de variables que PyArg_ParseTuple
	//
	//Si la vincion fuera void se debe realizar:
	// Py_INCREF(Py_None);
	// return Py_None;
	return PyInt_FromLong(i + j);
error:
	return 0;
}

//Tres funciones de prueba
PyObject * qc_date_2_string(PyObject * self, PyObject * args)
{
	long fecha;
	if (!PyArg_ParseTuple(args, "i", &fecha))
	{
		return NULL;
	}
	QCDate qcFecha{ fecha };
	std::string result = qcFecha.description();

	return Py_BuildValue("s", result.c_str());
}

PyObject * hello_world_nombre(PyObject * self, PyObject * args)
{
	char *nombre;
	if (!PyArg_ParseTuple(args, "s", &nombre))
	{
		return NULL;
	}

	//no se porque tengo que definir nombre como puntero...

	return Py_BuildValue("s", nombre);
}
PyObject * hello_world(PyObject * self, PyObject * args)
{
	return Py_BuildValue("s", "Hello World!");
}


//TEST LIST
PyObject * test_uso_list_r_int(PyObject * self, PyObject * args)
{
	PyObject * list;
	int aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		aux += PyInt_AsLong(PyList_GetItem(list, i));

	}

	return Py_BuildValue("i", aux);
}

//Toma lista de enteros y retorna la lista de enteros mas 1
PyObject * test_uso_list_r_list(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * result = PyList_New(0);
	PyObject * objeto_append;
	long aux;


	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		aux = PyInt_AsLong(PyList_GetItem(list, i)) + 1;
		objeto_append = PyInt_FromLong(aux);
		PyList_Append(result, objeto_append);
	}

	return result;
}

//Toma Lisla de listas de int (una matriz) y retorna la suma de sus elementos
PyObject * test_uso_tuples_r_int(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * list_in_list;
	int aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		//aux += PyInt_AsLong(PyList_GetItem(list, i));
		list_in_list = PyList_GetItem(list, i);
		for (int j = 0; j < PyList_Size(list_in_list); j++)
		{
			aux += PyInt_AsLong(PyList_GetItem(list_in_list, j));
		}

	}

	return Py_BuildValue("i", aux);
}
// toma una matriz de enteros y retorna la misma matriz mas uno
PyObject * test_uso_tuples_r_list_in_list(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * list_in_list;
	PyObject * result = PyList_New(0);
	PyObject * preresult;
	long aux = 0;

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		preresult = PyList_New(0);

		list_in_list = PyList_GetItem(list, i);
		for (int j = 0; j < PyList_Size(list_in_list); j++)
		{
			aux = PyInt_AsLong(PyList_GetItem(list_in_list, j)) + 1;
			PyList_Append(preresult, PyInt_FromLong(aux));
		}

		PyList_Append(result, preresult);

	}

	return result;
}

//tomo lista de listas de int y string y retorna la matris en 
//donde los valores int estan mas 1 y el string se concatena a CONCATENADO 
PyObject * test_uso_tuples_r_lil_string_int(PyObject * self, PyObject * args)
{
	PyObject * list;
	PyObject * list_in_list;
	PyObject * result = PyList_New(0);
	PyObject * preresult;
	long aux = 0;
	char label[100];

	if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &list)) return NULL;

	for (int i = 0; i < PyList_Size(list); i++)
	{
		preresult = PyList_New(0);
		list_in_list = PyList_GetItem(list, i);

		aux = PyInt_AsLong(PyList_GetItem(list_in_list, 0)) + 1;

		std::string auxlabel{ PyString_AsString(PyList_GetItem(list_in_list, 1)) };
		auxlabel += " CONCATENADO ";
		PyList_Append(preresult, PyInt_FromLong(aux));
		PyList_Append(preresult, PyString_FromString(auxlabel.c_str()));

		PyList_Append(result, preresult);

	}

	return result;
}


//PyMethodDef es un arreglo de las funciones 
// que que serán empaquetadas en el modulo de Python
PyMethodDef QC_DVE_Methods[] =
{
	//Primer argumento:		nombre con que se llamara a la función en python.
	//Segundo argumento:	funcion que se llamará (se castea a funcion de python)
	//tercer argumento:		flag que le dice al interprete la convencion utilizada por la funcion C
	//						 - METH_VARARGS: la funcion espera parámetros Python
	//						 - METH_KEYWORDS: The function expects three parameters: self, args, and a 
	//						   dictionary of all the keyword arguments. 
	//						   The flag is typically combined with METH_VARARGS,
	//						   and the parameters are typically processed using 
	//						   PyArg_ParseTupleAndKeywords()  
	//						 - 0: means that an obsolete variant of PyArg_ParseTuple() is used.
	{ "add_two_arguments", (PyCFunction)some_function, METH_VARARGS, 0 },
	{ "hello_world", (PyCFunction)hello_world, 0, 0 },
	{ "hello_world_nombre", (PyCFunction)hello_world_nombre, METH_VARARGS, 0 },
	{ "qc_date_to_string", (PyCFunction)qc_date_2_string, METH_VARARGS, 0 },
	{ "test_uso_list_r_int", (PyCFunction)test_uso_list_r_int, METH_VARARGS, 0 },
	{ "test_uso_list_r_list", (PyCFunction)test_uso_list_r_list, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_int", (PyCFunction)test_uso_tuples_r_int, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_list_in_list", (PyCFunction)test_uso_tuples_r_list_in_list, METH_VARARGS, 0 },
	{ "test_uso_tuples_r_lil_string_int", (PyCFunction)test_uso_tuples_r_lil_string_int, METH_VARARGS, 0 },
	{ 0, 0, 0, 0 }
};

//funcion __init__ python, Tecnicamente acá le estamos contruyendo el modulo pyd 
PyMODINIT_FUNC
initQC_DVE_PYBIND(void)
{
	PyObject *m;

	m = Py_InitModule("QC_DVE_PYBIND", QC_DVE_Methods);
	if (m == NULL)
		return;

}