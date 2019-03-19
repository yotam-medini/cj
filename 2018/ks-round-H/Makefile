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
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++14 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/bigbuttons: obj.d/bigbuttons.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bigbuttons-test: ${BINDIR}/bigbuttons bigbuttons-tiny.in bigbuttons-tiny.out
	timeout 8 ${BINDIR}/bigbuttons -naive bigbuttons-tiny.in bigbuttons-tiny.xnout
	diff bigbuttons-tiny.xnout bigbuttons-tiny.out
	timeout 8 ${BINDIR}/bigbuttons bigbuttons-tiny.in bigbuttons-tiny.xout
	diff bigbuttons-tiny.xout bigbuttons-tiny.out

${BINDIR}/mural: obj.d/mural.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

mural-test: ${BINDIR}/mural mural-tiny.in mural-tiny.out
	timeout 8 ${BINDIR}/mural mural-tiny.in mural-tiny.xout
	diff mural-tiny.xout mural-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
