## CPong: A C implementation of Pong

# Sources
FILES = main pong ansi_esc

# Configuration
BUILD_DIR = build
SRC_DIR = src
EXE_NAME = cpong
CFLAGS = -Wall -Wextra -g -fsanitize=address
CPPFLAGS = -I$(SRC_DIR)
LDFLAGS = -fsanitize=address
CC = clang
LD = clang

.PHONY: all
all: $(EXE_NAME)

OBJS = $(addprefix build/,$(addsuffix .o,$(FILES)))

$(OBJS): | $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

$(EXE_NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(EXE_NAME)
