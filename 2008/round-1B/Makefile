#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

# LIBDIR=/usr/lib/i386-linux-gnu
ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
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

${BINDIR}/crop: obj.d/crop.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ $(LIBGMP)

crop-test: ${BINDIR}/crop crop-tiny.in crop-tiny.out
	${BINDIR}/crop crop-tiny.in crop-tiny.xout
	diff crop-tiny.xout crop-tiny.out

${BINDIR}/numsets: obj.d/numsets.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

numsets-test: ${BINDIR}/numsets numsets-tiny.in numsets-tiny.out
	${BINDIR}/numsets numsets-tiny.in numsets-tiny.xout
	diff numsets-tiny.xout numsets-tiny.out

${BINDIR}/mousetrap: obj.d/mousetrap.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

mousetrap-test: ${BINDIR}/mousetrap mousetrap-tiny.in mousetrap-tiny.out
	${BINDIR}/mousetrap mousetrap-tiny.in mousetrap-tiny.xout
	diff mousetrap-tiny.xout mousetrap-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
