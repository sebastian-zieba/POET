#include<Python.h>
#include<numpy/arrayobject.h>
#include<math.h>
#include<omp.h>

#define IND(a,i) *((double *)(a->data+i*a->strides[0]))

static PyObject *selramp(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *selramp(PyObject *self, PyObject *args, PyObject *keywds)
{
  PyObject *etc;
  PyArrayObject *x,*y, *rampparams;
  double goal,r0,r1,r2,x0,pm;
  int i;
  npy_intp dims[1];

  //  etc = PyList_New(0);

  static char *kwlist[] = {"rampparams","x","etc",NULL};

  if(!PyArg_ParseTupleAndKeywords(args,keywds,"OO|O",kwlist,&rampparams,&x,&etc))
    {
      return NULL;
    }

  goal = IND(rampparams,0);
  r0   = IND(rampparams,1);
  r1   = IND(rampparams,2);
  r2   = IND(rampparams,3);
  x0   = IND(rampparams,4);
  pm   = IND(rampparams,5);

  dims[0] = x->dimensions[0];

  y = (PyArrayObject *) PyArray_SimpleNew(1,dims,PyArray_DOUBLE);
  #pragma omp parallel for
  for(i=0;i<dims[0];i++)
    {
      IND(y,i) = goal + pm*exp(-r0*IND(x,i)+r1) + r2*(IND(x,i)-x0);
    }
  return PyArray_Return(y);
}

static PyMethodDef selramp_methods[] = {
  {"selramp",(PyCFunction)selramp,METH_VARARGS|METH_KEYWORDS},{NULL}};

void initselramp(void)
{
  Py_InitModule("selramp",selramp_methods);
  import_array();
}
