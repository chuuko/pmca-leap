# マクロ定義部
CC	= gcc
CFLAGS	=  -fPIC -O2 -msse
LDFLAGS	= 
DLLFLAGS = -shared
INCLUDES = -IPython32\include
LIBS	= -lm -lopengl32 -lglu32 -lSDL -lSDL_image
MYLIBS  = -LD mlib_PMD.dll
PYTHON = -LPython32\libs -LD Python32\DLLs\python3.dll

TERGET0 = mlib_PMD.dll
OBJS0	= mlib_PMD_edit01.o mlib_PMD_rw01.o

TERGET1 = PMCA.pyd
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
