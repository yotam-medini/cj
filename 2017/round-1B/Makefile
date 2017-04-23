#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

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

${BINDIR}/cruise: obj.d/cruise.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ $(LIBGMP)

cruise-test: ${BINDIR}/cruise cruise-tiny.in cruise-tiny.out
	${BINDIR}/cruise cruise-tiny.in cruise-tiny.xout
	diff cruise-tiny.xout cruise-tiny.out

${BINDIR}/stable: obj.d/stable.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

stable-test-small: ${BINDIR}/stable stable-small.in stable-small.out
	${BINDIR}/stable stable-small.in stable-small.xout
	diff stable-small.xout stable-small.out

stable-test: ${BINDIR}/stable stable-tiny.in stable-tiny.out
	${BINDIR}/stable stable-tiny.in stable-tiny.xout
	diff stable-tiny.xout stable-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
