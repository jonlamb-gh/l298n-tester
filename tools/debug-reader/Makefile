##########################################################
# makefile for usb-tester-reader
##########################################################

TARGET := bin/usb-tester-reader

SRCS := src/hid.c src/main.c

OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.dep)
XDEPS := $(wildcard $(DEPS))

CC = gcc

CCFLAGS = -std=gnu99

CCFLAGS += -Wall -Wextra -Werror \
		-Wformat=2 -Wno-unused-parameter -Wshadow \
		-Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		-Wnested-externs -Wmissing-include-dirs

LDFLAGS =

INCLUDES = -Iinclude -I../../firmware/include

LIBS = -lusb

all: dirs $(TARGET)

ifneq ($(XDEPS),)
include $(XDEPS)
endif

dirs::
	mkdir -p bin

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDES) -o $@ -c $<

$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDES) -MM $< > $@

install: all
	cp $(TARGET) ~/bin/

clean:
	-rm -f src/*.o
	-rm -f src/*.dep
	-rm -f $(TARGET)
