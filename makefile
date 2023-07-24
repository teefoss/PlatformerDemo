TARGET	= $(shell basename $(CURDIR))
CC		= clang
CFLAGS	= -Wall -Wextra -Werror -Wshadow -g
DIR		= /Users/tomf/dev
LIBS	= -L$(DIR)/lib
INCL	= -I$(DIR)/include
LINK	= -lSDL2 -lSDL2_image
SRC		= $(wildcard *.c)
OBJ_DIR = ./obj
OBJ		= $(SRC:%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS) $(LINK)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCL) -o $@ -c $<

.PHONY: clean
clean:
	-@rm -rf $(TARGET) $(OBJ_DIR)
