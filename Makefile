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

SERVER_SRCS = Server.cpp SignalTools.cpp VerosimMoosInterface.cpp
SERVER_HDRS = Client.h Server.h SignalTools.h CommandCallbackHandler.h VerosimAUVCommProt.h
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

CLIENT_HDRS = Client.h  SignalTools.h VerosimAUVCommProt.h
CLIENT_SRCS = Client.cpp SignalTools.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

#EXE = VerosimAUVinterfaceServer
SERVER_EXE = VerosimMoosInterface
CLIENT_EXE = DemoClient

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE):	$(SERVER_OBJS) $(SERVER_HDRS)
	$(CC) -o $(SERVER_EXE) $(SERVER_OBJS) $(LFLAGS) $(LIBS)    

$(CLIENT_EXE): $(CLIENT_OBJS) $(CLIENT_HDRS)
	$(CC) -o $(CLIENT_EXE) $(CLIENT_OBJS) $(LFLAGS) $(LIBS)    
	
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

tar:
	tar -czf $(SERVER_EXE)_$(CLIENT_EXE).tar.gz $(SOURCES) $(HEADERS) Makefile

clean:
	rm -f $(OBJ)
	rm -f $(EXE)

OBJ = $(SERVER_OBJS) $(CLIENT_OBJS)
EXE = $(SERVER_EXE) $(CLIENT_EXE)

