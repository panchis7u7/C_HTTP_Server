CC=gcc
CFLAGS=-Wall -Wextra -Werror -pthread
PROG=Stratus
MYSQLCFLAGS= `mysql_config --cflags`
MYSQLLIBS= `mysql_config --libs`
INCLUDES= /usr/include/mysql/mysql.h
SRC=src
OBJ=obj
BIN=bin
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -g -o $(BIN)/$@ $(INCLUDES) $(MYSQLCFLAGS) $^ $(MYSQLLIBS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -r $(BIN)/* $(OBJ)/*
	rm -f Server
