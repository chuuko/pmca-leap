#include "PMCA.h"

MODEL g_model[MODEL_COUNT];
LIST list;
SDL_Thread *viewer_th;

PyObject* CreateViewerThread(PyObject *self, PyObject *args)
{
	viewer_th = SDL_CreateThread(&viewer_thread, NULL);
	Py_RETURN_NONE;
}

PyObject* WaitViewerThread(PyObject *self, PyObject *args)
{
	SDL_WaitThread(viewer_th, NULL);
	Py_RETURN_NONE;
}

PyObject* QuitViewerThread(PyObject *self, PyObject *args)
{
	myflags.quit = 1;
	SDL_WaitThread(viewer_th, NULL);
	
	Py_RETURN_NONE;
}

PyObject* KillViewerThread(PyObject *self, PyObject *args)
{
	SDL_KillThread(viewer_th);
	Py_RETURN_NONE;
}

PyObject* GetViewerThreadState(PyObject *self, PyObject *args)
{
	return Py_BuildValue("i", myflags.quit);
}

PyObject* show3Dview(PyObject *self, PyObject *args)
{
	if(myflags.quit == 1){
		viewer_th = SDL_CreateThread(&viewer_thread, NULL);
	}
	Py_RETURN_NONE;
}

