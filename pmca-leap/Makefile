# マクロ定義部
CC	= gcc
CFLAGS	=  -fPIC -O2 -msse -ggdb
LDFLAGS	= 
DLLFLAGS = -shared
INCLUDES = -I/usr/include/python3.4m
LIBS	= -lm -lGLU -lGL -lSDL -lSDL_image
MYLIBS  = -LD mlib_PMD.so
PYTHON = -L/usr/lib64/python3.4/config-3.4m -lpython3.4m

TERGET0 = mlib_PMD.so
OBJS0	= mlib_PMD_edit01.o mlib_PMD_rw01.o

TERGET1 = PMCAmodule.so PMCA.so
OBJS1	= PMCA_SDLMod.o PMCA_PyMod.o PMCA_view.o

# 生成規則部
all:$(TERGET0) $(TERGET1)

$(TERGET0): $(OBJS0)
	$(CC) $(DLLFLAGS) -o $@ $(OBJS0)
	
$(TERGET1): $(OBJS1)
	-$(CC) $(DLLFLAGS) $(LDFLAGS) -o $@ $(OBJS1) $(PYTHON) $(LIBS) $(MYLIBS)
	

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.coff : %.rc
	$(RC) -i $< -o $@

clean:
	-$(RM) $(TARGET1) $(OBJS1)

mlib_PMD_edit01.o: mPMD.h
mlib_PMD_rw01.o  : mPMD.h

PMCA_main.o : PMCA.h mPMD.h
PMCA_PyMod.o: PMCA.h mPMD.h
PMCA_view.o : PMCA.h mPMD.h
