COMMON_CFLAGS=-Wall
#DBGFLAGS=-D_DEBUG -Os
DBGFLAGS=-Os
OBJS=binflash.o plscsi.o drive.o necinterface.o necinternal.o
OUTPUT=necflash

dos: CFLAGS_OS= -s
dos: $(OUTPUT)

linux: CFLAGS_OS= -s -static
linux: $(OUTPUT)

mac:
	make -f Makefile.mac

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CXX) $(ARCHFLAGS) $(COMMON_CFLAGS) $(CFLAGS_OS) -o $(OUTPUT) $(OBJS)

plscsi.o:
	cd plscsi;make $(PLSCSIMAKEFILE);cd ..

%.o: %.cpp
	$(CXX) $(COMMON_CFLAGS) $(ARCHFLAGS) $(DBGFLAGS) -c $<

clean:
	find . -name "*.o" -exec $(RM) {} \;
