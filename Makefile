CC=gcc
CFLAGS= -Wall -Wconversion -g 
LIBS=-lm -lpthread
DEPS=

BUILD_DIR := build

SRCS := osmprun.c echoall.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))

$(BUILD_DIR)/%.o: %.c $(DEPS)
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

all: echoall osmprun

osmprun: osmprun.c
	$(CC) $(CFLAGS)  $^ $(LIBS) -o $@

echoall: echoall.c
	$(CC) $(CFLAGS)  $^ $(LIBS) -o $@



.PHONY: clean
.PHONY: all

clean:
	rm -f $(BUILD_DIR)/*.o *~ core

test:
	./osmprun
	./echoall Echoall-Test

