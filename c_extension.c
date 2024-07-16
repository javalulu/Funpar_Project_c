//
// Created by HUAWEI on 8/7/24.
//

#include "Python.h"
#include "stdlib.h"
#include "omp.h"

#define MAX_SIZE 10000*10000
typedef double BASE_TYPE;
BASE_TYPE row_major[MAX_SIZE];
BASE_TYPE column_major[MAX_SIZE];

BASE_TYPE **init_result_array(int total_rows, int total_columns) {
    BASE_TYPE **result_array = (BASE_TYPE **)calloc(total_rows, sizeof(BASE_TYPE *));
    for(int row = 0; row < total_rows; row++) {
        result_array[row] = (BASE_TYPE *)calloc(total_columns, sizeof(BASE_TYPE));
    }
    return result_array;
}

BASE_TYPE **convert(PyObject *ndimarray, int rows, int columns) {
    BASE_TYPE **c_array = init_result_array(rows, columns);
    PyObject *current_row;
    for (int i = 0; i < rows; ++i) {
        current_row = PyList_GetItem(ndimarray, i);
        for (int j = 0; j < columns; ++j) {
            c_array[i][j] = (BASE_TYPE)PyLong_AsLong(PyList_GetItem(current_row, j));
        }
    }
    return c_array;
}

void transform_row_major(BASE_TYPE **ndimarray, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            row_major[i * columns + j] = ndimarray[i][j];
        }
    }
}

void transform_column_major(BASE_TYPE **ndimarray, int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            column_major[j * rows + i] = ndimarray[i][j];
        }
    }
}

PyObject* build_python_array(BASE_TYPE** result_array, int rows, int columns) {
    PyObject *item;
    PyObject *pyResult = PyList_New(rows);
    for (int i= 0; i< rows; ++i) {
        item = PyList_New(columns);
        for (int j= 0; j< columns; ++j) {
            PyList_SetItem(item, j, PyLong_FromLong(result_array[i][j]));
        }
        PyList_SetItem(pyResult, i, item);
    }
    return pyResult;
}

PyObject* dot_product_optimized_parallel(PyObject* self, PyObject* args) {
    PyObject *mat1;
    PyObject *mat2;

    if (!PyArg_ParseTuple(args, "O|O", &mat1, &mat2)){
        return NULL;
    }
    int mat1_rows, mat1_columns, mat2_rows, mat2_columns;
    mat1_rows = PyObject_Length(mat1);
    mat1_columns = PyObject_Length(PyList_GetItem(mat1, 0));
    mat2_rows = PyObject_Length(mat2);
    mat2_columns = PyObject_Length(PyList_GetItem(mat2, 0));
    BASE_TYPE **mat1_c = convert(mat1, mat1_rows, mat1_columns);
    BASE_TYPE **mat2_c = convert(mat2, mat2_rows, mat2_columns);
    transform_row_major(mat1_c, mat1_rows, mat1_columns);
    transform_column_major(mat2_c, mat2_rows, mat2_columns);
    BASE_TYPE **result = init_result_array(mat1_rows, mat2_columns);

    int block_size = 64; // Experiment with different block sizes

#pragma omp parallel for collapse(2) schedule(dynamic)
    for(int ii = 0; ii < mat1_rows; ii += block_size) {
        for(int jj = 0; jj < mat2_columns; jj += block_size) {
            for(int i = ii; i < ii + block_size && i < mat1_rows; i++) {
                int iOff = i * mat1_columns;
                for(int j = jj; j < jj + block_size && j < mat2_columns; j++) {
                    int jOff = j * mat2_rows;
                    BASE_TYPE tot = 0;
                    for(int k = 0; k < mat2_rows; k++) {
                        tot += row_major[iOff + k] * column_major[jOff + k];
                    }
                    result[i][j] += tot;
                }
            }
        }
    }

    return Py_BuildValue("O", build_python_array(result, mat1_rows, mat2_columns));
}


static PyMethodDef module_methods[] = {
        {"dot_product_optimized_parallel", (PyCFunction) dot_product_optimized_parallel, METH_VARARGS, "Parallel optimized version of dot_product"},
};

static struct PyModuleDef cModPyDem = {
        PyModuleDef_HEAD_INIT,
        "c_extension",
        "",
        -1,
        module_methods
};


PyMODINIT_FUNC PyInit_c_extension(void) {
    return PyModule_Create(&cModPyDem);
}