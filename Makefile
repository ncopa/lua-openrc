
VERSION = 0.2
GIT_REV		:= $(shell test -d .git && git describe || echo exported)
ifneq ($(GIT_REV), exported)
FULL_VERSION    := $(GIT_REV)
FULL_VERSION    := $(patsubst v%,%,$(FULL_VERSION))
else
FULL_VERSION    := $(VERSION)
endif

LUA_VERSION=5.2
LUA_PKG=lua$(LUA_VERSION)
LUA_CFLAGS=$(shell pkg-config --cflags $(LUA_PKG))
OBJS = rc.o
LIBS = -lrc

DEBUGFLAGS = -g -Wall -Werror
CFLAGS += $(DEBUGFLAGS)
CFLAGS += -DVERSION=\"$(FULL_VERSION)\"
CFLAGS += $(LUA_CFLAGS)

LDFLAGS += -L/lib

all:	rc.so


rc.so: $(OBJS)
	$(CC) $(LDFLAGS) -o $@  -fPIC -shared $^ $(LIBS)

clean:
	rm -f rc.so $(OBJS)
