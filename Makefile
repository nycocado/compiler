CC = gcc
CFLAGS = -Wall -g -Iinclude
LEX = flex
YACC = bison -d

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/analisador

# Objs based on source files
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/ast.o $(OBJ_DIR)/translator.o \
       $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o

all: directories $(TARGET)

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/lex.yy.o: $(SRC_DIR)/lex.yy.c $(SRC_DIR)/parser.tab.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/parser.tab.o: $(SRC_DIR)/parser.tab.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l $(SRC_DIR)/parser.tab.h
	$(LEX) -o $@ $<

$(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	$(YACC) -o $(SRC_DIR)/parser.tab.c $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h

format:
	clang-format -i $(SRC_DIR)/*.c $(INC_DIR)/*.h

.PHONY: all clean format directories
