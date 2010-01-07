
VERSION = 0.1

OBJS = rc.o
LIBS = -lrc

DEBUGFLAGS = -g -Wall -Werror
CFLAGS += $(DEBUGFLAGS)
CFLAGS += -DVERSION=\"$(VERSION)\"

LDFLAGS += -L/lib

all:	rc.so


rc.so: $(OBJS)
	$(CC) $(LDFLAGS) -o $@  -fPIC -shared $^ $(LIBS)

clean:
	rm -f rc.so $(OBJS)
