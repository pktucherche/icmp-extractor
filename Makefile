CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

SRCDIR = src
OBJDIR = obj
INCDIR = include

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
BIN = exec

all: $(OBJ) $(BIN)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< -I$(INCDIR)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN) 

clean:
	rm -f $(OBJ)
	rm -f $(BIN)

.PHONY: clean
.PHONY: all

