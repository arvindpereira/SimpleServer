# Author: Arvind Pereira
# This project depends upon the ArvindsTools library which can be obtained from:
# https://github.com/arvindpereira/ArvindsTools

INCLUDES = -I ./ -I/usr/local/include -I../ArvindsTools/
LIBS = -L/usr/lib/ -lm -pthread -lpthread

CC = g++
DEBUG = -g
LFLAGS = -Wall $(DEBUG)
CFLAGS = -Wall -c -D_POSIX_PTHREAD_SEMANTICS

SERVER_SRCS = VerosimMoosInterface.cpp
SERVER_HDRS = VerosimAUVCommProt.h
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

CLIENT_HDRS = VerosimAUVCommProt.h
CLIENT_SRCS = DemoClient.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

SERVER_EXE = VerosimMoosInterface
CLIENT_EXE = DemoClient
TOPLEVEL_FOLDER = SimpleServer

ARVINDS_TOOLS_LIBS = -L../ArvindsTools/ -l_arvinds_tools


all: $(SERVER_EXE) $(CLIENT_EXE)
	@echo "-------------------------Done------------------------------"
	@echo "Type: 'make docs' for documentation"
	@echo "Type: 'make tar'  for a code snapshot to be saved to tar.gz"

$(SERVER_EXE):	$(SERVER_OBJS) $(SERVER_HDRS)
	$(CC) -o $(SERVER_EXE) $(SERVER_OBJS) $(LFLAGS) $(LIBS) $(ARVINDS_TOOLS_LIBS) 

$(CLIENT_EXE): $(CLIENT_OBJS) $(CLIENT_HDRS)
	$(CC) -o $(CLIENT_EXE) $(CLIENT_OBJS) $(LFLAGS) $(LIBS) $(ARVINDS_TOOLS_LIBS) 
	
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

tar:
	make clean; cd ..; tar -czf $(TAR_FILE_NAME) $(TOPLEVEL_FOLDER)
	cd ..; mv *$(TOPLEVEL_FOLDER).tar.gz $(TOPLEVEL_FOLDER)/

clean:
	rm -f $(OBJ)
	rm -f $(EXE)
	rm -rf docs
	
TAR_FILE_NAME = `date +'%Y%m%d_%H%M%S_'`$(TOPLEVEL_FOLDER).tar.gz

OBJ = $(SERVER_OBJS) $(CLIENT_OBJS)
EXE = $(SERVER_EXE) $(CLIENT_EXE)

SOURCES = $(SERVER_SRCS) $(CLIENT_SRCS)
HEADERS = $(SERVER_HDRS) $(CLIENT_HDRS)

docs: $(SOURCES) $(HEADERS)
	if command -v doxygen; then doxygen Doxyfile; \
	else echo "Doxygen not found. Not making documentation."; fi

