# 

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/pingpong: obj.d/pingpong.o obj.d/ratgeo.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

pingpong-test: ${BINDIR}/pingpong pingpong-tiny.in pingpong-tiny.out
	${BINDIR}/pingpong pingpong-tiny.in pingpong-tiny.xout
	diff pingpong-tiny.xout pingpong-tiny.out
	${BINDIR}/pingpong pingpong-tiny.in pingpong-tiny.xlout 0 -large
	diff pingpong-tiny.xlout pingpong-tiny.out




clean:
	rm -f ${OBJS}

-include ${DEPS}


