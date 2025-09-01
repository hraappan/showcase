CC = gcc
CFLAGS = -Wall -Wextra -Iinc -fsanitize=address -lm -lcrypto -lssl
SRC_DIR = src
OBJ_DIR = obj

# Yhteiset lähteet ilman main-funktiota
COMMON_SRCS = $(filter-out $(SRC_DIR)/main-%.c $(SRC_DIR)/test-%.c, $(wildcard $(SRC_DIR)/*.c))
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))

# Mainit
SERVER_MAIN = $(OBJ_DIR)/main-server.o
CLIENT_MAIN = $(OBJ_DIR)/main-client.o
TEST_MAIN   = $(OBJ_DIR)/test-all.o

# Targetit
TARGET_SERVER = server
TARGET_CLIENT = client
TARGET_TEST   = test

# Luo objektihakemisto
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Käännä yleiset lähteet
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Linkkaa server
$(TARGET_SERVER): $(COMMON_OBJS) $(SERVER_MAIN)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(SERVER_MAIN) -o $@

# Linkkaa client
$(TARGET_CLIENT): $(COMMON_OBJS) $(CLIENT_MAIN)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(CLIENT_MAIN) -o $@

# Linkkaa test cmocka-kirjastolla
$(TARGET_TEST): $(COMMON_OBJS) $(TEST_MAIN)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(TEST_MAIN) -lcmocka -o $@

.PHONY: all server client test clean

all: server client test

server: $(TARGET_SERVER)
client: $(TARGET_CLIENT)
test: $(TARGET_TEST)

clean:
	rm -rf $(OBJ_DIR) $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_TEST)
