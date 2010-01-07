
VERSION = 0.1
GIT_REV		:= $(shell test -d .git && git describe || echo exported)
ifneq ($(GIT_REV), exported)
FULL_VERSION    := $(GIT_REV)
FULL_VERSION    := $(patsubst v%,%,$(FULL_VERSION))
else
FULL_VERSION    := $(VERSION)
endif



OBJS = rc.o
LIBS = -lrc

DEBUGFLAGS = -g -Wall -Werror
CFLAGS += $(DEBUGFLAGS)
CFLAGS += -DVERSION=\"$(FULL_VERSION)\"

LDFLAGS += -L/lib

all:	rc.so


rc.so: $(OBJS)
	$(CC) $(LDFLAGS) -o $@  -fPIC -shared $^ $(LIBS)

clean:
	rm -f rc.so $(OBJS)
