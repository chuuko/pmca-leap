#include <Python.h>


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_thread.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "mPMD.h"
#define SCALE (2.0 * 3.14159265358979323846)
#define MODEL_COUNT 16

typedef struct{
	float col[4];
	char texname[128];
	int texsize[2];
	GLubyte *texbits;
}DSP_MAT;

typedef struct{
	float *loc;
	float *nor;
	float *uv;
	//unsigned int *index;
	int mats_c;
	DSP_MAT *mats;
	GLuint *texid;
}DSP_MODEL;

typedef struct{
	int button1;
	int button2;
	int button3;
	int model_lock;
	int quit;
}FLAGS;

typedef struct{
	/* EBhE@ */
	int width;
	int height;
	
	/*NbNJ[\W*/
	int sx;
	int sy;
	
	/*fr[]*/
	double rt[16];
	double cq[4];
	double tq[4];
	
	/*s*/
	double move[3];
	
	/*TCY*/
	double scale;
	
	/*\*/
	int show_axis;
}VIEW_STATE;


/*****************************************************************/
/*PMDr[AA*/
int viewer_thread();
void quit( int code );
void qmul(double r[], const double p[], const double q[]);
void qrot(double r[], double q[]);
void* model_mgr(int flag, int num, void* p);
int render_model(int num);
int load_texture(MODEL *model);
int load_tex(MODEL *model, DSP_MODEL *dsp_model);
int make_dsp_model(MODEL *model, DSP_MODEL *dsp_model);

PyObject* CreateViewerThread(PyObject *self, PyObject *args);
PyObject* WaitViewerThread(PyObject *self, PyObject *args);
PyObject* QuitViewerThread(PyObject *self, PyObject *args);
PyObject* KillViewerThread(PyObject *self, PyObject *args);
PyObject* GetViewerThreadState(PyObject *self, PyObject *args);
PyObject* show3Dview(PyObject *self, PyObject *args);

extern FLAGS myflags;

/*****************************************************************/
/*PythonAPIA*/
PyMODINIT_FUNC PyInit_PMCA(void);
extern MODEL g_model[16];
extern LIST list;


