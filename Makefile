CC=g++

BIT_DEPTH_FLAG=

APP_CFLAGS=$(BIT_DEPTH_FLAG) -g -Wno-unused -Wall -Ofast -DNDEBUG
APP_OBJS_CFLAGS=$(BIT_DEPTH_FLAG) -g -c -Wno-unused -Wall -Ofast -DNDEBUG -std=c++11

SOURCES=stdafx.cpp\
	main.cpp\
	tcp_server_c.cpp\
	server_impl_c.cpp\
	client_connection_c.cpp
COMMON_SOURCES=
THIRD_PARTY_SOURCES=

APP_SOURCES=$(SOURCES) $(COMMON_SOURCES) $(THIRD_PARTY_SOURCES) $(MAIN_SOURCES)

LIBS_FLAGS_ENUM=-l"pthread" -l"boost_thread" -l"boost_filesystem" -l"boost_system"
INCLUDE_DIRECTORIES_ENUM=-I.

OBJ_DIR=obj/
OUTPUT_DIR=bin/
OUTPUT_APP_NAME="boost_socket_server"

APP_OBJECTS=$(APP_SOURCES:.cpp=.o)

OBJECTS_TO_CLEAN := $(addsuffix .o,$(basename $(APP_SOURCES)))

all: $(APP_SOURCES) app

app: $(APP_OBJECTS)
	$(CC) $(APP_CFLAGS) $(APP_OBJECTS) -o $(OUTPUT_DIR)$(OUTPUT_APP_NAME) $(LIBS_FLAGS_ENUM)

.cpp.o:
	@echo [assemble object file. input file: $<, output file: $@]
	$(CC) $(APP_OBJS_CFLAGS) $(INCLUDE_DIRECTORIES_ENUM) $< -o $@

run:

clean: clean_objs
	rm -rf *.o ./bin/$(OUTPUT_APP_NAME)

clean_objs:
	for i in $(OBJECTS_TO_CLEAN); do\
		echo [delete object file $$i];\
		rm -rf $$i;\
	done
