#include <atm_linux_config.h>
#include <module_utils.h>


PyObject *
native_foo(PyObject *self, PyObject *args)
{
    const char *echo;
    if (!PyArg_ParseTuple(args, "s", &echo))
        return NULL;
    return PyString_FromString(echo);
}
