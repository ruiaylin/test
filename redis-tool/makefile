#makefile for those who have many excutable files

.SUFFIXES: .sh .h .c .cc .cpp

CXX	        = gcc
#CXXFLAGS	= -ggdb3 -Wall -Werror
CXXFLAGS	= -std=c99 -pedantic -c -fPIC -Wstrict-prototypes -Wwrite-strings -g -ggdb #-Wall -W 
CXXFLAG     = -fPIC  -Wall -W -Wstrict-prototypes -Wwrite-strings -g -ggdb 

INC_DIR		= inc
SRC_DIR		= src
OBJ_DIR		= obj
LIB_DIR		= lib
BIN_DIR		= bin
BIN	        = redis-dump redis-dump-key redis-migrate

BASE_PATH	=

INC	        = -I$(INC_DIR) -I$(INC_DIR)/hiredis

# rpath把lib的绝对路径放在elf文件中  -Wl,-rpath=./$(LIB_DIR) 
LIB	        =  -L./$(LIB_DIR) -lhiredis

all: clean debug release # strip

debug release gprof: dir $(addprefix $(BIN_DIR)/,$(BIN))


#@ do not outoupt this command - ingnore its error and continue to make
dir:
	@-mkdir -p $(INC_DIR)
	@-mkdir -p $(SRC_DIR)
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(LIB_DIR)
	@-mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $(INC) -c $<

$(BIN_DIR)/redis-dump:$(addprefix $(OBJ_DIR)/, tool.o redis-dump.o)
	$(CXX) $(CXXFLAG) $(CXX_MACRO) -o $@ $^ $(LIB)

$(BIN_DIR)/redis-dump-key:$(addprefix $(OBJ_DIR)/, tool.o redis-dump-key.o)
	$(CXX) $(CXXFLAG) $(CXX_MACRO) -o $@ $^ $(LIB)

$(BIN_DIR)/redis-migrate:$(addprefix $(OBJ_DIR)/, tool.o redis-migrate.o)
	$(CXX) $(CXXFLAG) $(CXX_MACRO) -o $@ $^ $(LIB)

strip:
	strip $(addprefix $(BIN_DIR)/,$(BIN))

clean:
	rm -f $(addprefix $(BIN_DIR)/,$(BIN)) $(OBJ_DIR)/*.o

