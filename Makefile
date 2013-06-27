# Author: Arvind Pereira

# Modify the includes, sources, headers, executable and
# other flags accordingly!
INCLUDES = -I ./ -I/usr/local/include
LIBS = -L/usr/lib/ -lm -pthread -lpthread

CC = g++
DEBUG = -g
LFLAGS = -Wall $(DEBUG)
CFLAGS = -Wall -c

#SOURCES = Server.cpp \
#	  SignalTools.cpp \
#	  VerosimAUVInterface.cpp

SOURCES = Server.cpp \
	  SignalTools.cpp \
	  VerosimMoosInterface.cpp

HEADERS = Client.h \
	  Server.h \
	  SignalTools.h \
	  VerosimAUVCommProt.h

OBJ = $(SOURCES:.cpp=.o)

#EXE = VerosimAUVinterfaceServer
EXE = VerosimMoosInterface

all: $(EXE)

$(EXE):	$(OBJ) $(HEADERS)
	$(CC) -o $(EXE) $(OBJ) $(LFLAGS) $(LIBS)    

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

tar:
	tar -czf $(EXE).tar.gz $(SOURCES) $(HEADERS) Makefile

clean:
	rm -f $(OBJ)
	rm -f $(EXE)
