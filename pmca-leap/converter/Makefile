# É}ÉNÉçíËã`ïî
CC	= gcc
CFLAGS	= -Wall
LDFLAGS	= -Wall
DLLFLAGS = -Wall -shared
INCLUDES = 
LIBS	= 
MYLIBS  = 
RC = windres
RM = del

TERGET1 = PMCA_1.0-2.0converter
OBJS1	= PMCA_main.o PMCA_loadconf.o

# ê∂ê¨ãKë•ïî
all: $(TERGET1)

$(TERGET1): $(OBJS1) $(RES1)
	-$(CC) $(LDFLAGS) -o $@ $(OBJS1) $(LIBS) $(MYLIBS)

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.coff : %.rc
	$(RC) -i $< -o $@

clean:
	-$(RM) $(TARGET1) $(OBJS1)


PMCA_main.o      : mPMD.h PMCA.h
PMCA_loadconf.o  : mPMD.h PMCA.h

