#ifndef MATPLOTLIBCPP_H
#define MATPLOTLIBCPP_H

#include <Python.h>
#include <vector>
#include <string>
#include <iostream>

namespace matplotlibcpp {

    // Función para inicializar Python
    void init() {
        Py_Initialize();
    }

    // Función para finalizar la ejecución de Python
    void finalize() {
        Py_Finalize();
    }

    // Función para crear una figura
    void figure() {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }
        PyObject* func = PyObject_GetAttrString(matplotlib, "figure");
        if (func && PyCallable_Check(func)) {
            PyObject_CallObject(func, NULL);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para graficar líneas
    void plot(const std::vector<double>& x, const std::vector<double>& y) {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }

        PyObject* func = PyObject_GetAttrString(matplotlib, "plot");
        if (func && PyCallable_Check(func)) {
            PyObject* args = PyTuple_Pack(2, 
                PyList_FromVector(x), 
                PyList_FromVector(y));
            PyObject_CallObject(func, args);
            Py_XDECREF(args);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para mostrar el gráfico
    void show() {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }
        PyObject* func = PyObject_GetAttrString(matplotlib, "show");
        if (func && PyCallable_Check(func)) {
            PyObject_CallObject(func, NULL);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para establecer el título del gráfico
    void title(const std::string& title) {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }
        PyObject* func = PyObject_GetAttrString(matplotlib, "title");
        if (func && PyCallable_Check(func)) {
            PyObject* arg = PyUnicode_FromString(title.c_str());
            PyObject_CallObject(func, PyTuple_Pack(1, arg));
            Py_XDECREF(arg);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para definir etiquetas en los ejes
    void xlabel(const std::string& label) {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }
        PyObject* func = PyObject_GetAttrString(matplotlib, "xlabel");
        if (func && PyCallable_Check(func)) {
            PyObject* arg = PyUnicode_FromString(label.c_str());
            PyObject_CallObject(func, PyTuple_Pack(1, arg));
            Py_XDECREF(arg);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para definir etiquetas en los ejes
    void ylabel(const std::string& label) {
        PyObject* matplotlib = PyImport_ImportModule("matplotlib.pyplot");
        if (!matplotlib) {
            std::cerr << "Error al importar matplotlib!" << std::endl;
            return;
        }
        PyObject* func = PyObject_GetAttrString(matplotlib, "ylabel");
        if (func && PyCallable_Check(func)) {
            PyObject* arg = PyUnicode_FromString(label.c_str());
            PyObject_CallObject(func, PyTuple_Pack(1, arg));
            Py_XDECREF(arg);
        }
        Py_XDECREF(func);
        Py_XDECREF(matplotlib);
    }

    // Función para crear una lista de datos en Python desde un vector de C++
    PyObject* PyList_FromVector(const std::vector<double>& vec) {
        PyObject* pyList = PyList_New(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            PyList_SetItem(pyList, i, PyFloat_FromDouble(vec[i]));
        }
        return pyList;
    }
}

#endif // MATPLOTLIBCPP_H
