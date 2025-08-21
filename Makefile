CC = gcc
CFLAGS = -Wall -Wextra -Iinc -fsanitize=address -lm -lcrypto
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = linked_list

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	CFLAGS="$(CFLAGS)"
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)