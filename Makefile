CC=gcc
CFLAGS= -Wall -Wconversion -g 
LIBS=-lm -lpthread
DEPS=

BUILD_DIR := build

SRCS := osmprun.c echoall.c osmpexecutable.c
OBJS := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(SRCS)))

$(BUILD_DIR)/%.o: %.c $(DEPS)
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

all: echoall osmprun osmpexecutable

osmprun: osmprun.c OSMP.c
	$(CC) $(CFLAGS)  $^ $(LIBS) -o $@ -lrt

echoall: echoall.c
	$(CC) $(CFLAGS)  $^ $(LIBS) -o $@

osmpexecutable: osmpexecutable.c OSMP.c
	$(CC) $(CFLAGS)  $^ $(LIBS) -o $@ -lrt

.PHONY: clean
.PHONY: all

clean:
	rm -f $(BUILD_DIR)/*.o *~ core

test:
	./osmprun 2 ./osmpexecutable
	./echoall Echoall-Test

