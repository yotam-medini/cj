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

${BINDIR}/cheating: obj.d/cheating.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cheating-test: ${BINDIR}/cheating cheating-tiny.in cheating-tiny.out
	${BINDIR}/cheating cheating-tiny.in cheating-tiny.xout
	diff cheating-tiny.xout cheating-tiny.out

${BINDIR}/triangles: obj.d/triangles.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

triangles-test: ${BINDIR}/triangles triangles-tiny.in triangles-tiny.out
	${BINDIR}/triangles -small triangles-tiny.in triangles-tiny.xout
	diff triangles-tiny.xout triangles-tiny.out

${BINDIR}/starwars: obj.d/starwars.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

starwars-test: ${BINDIR}/starwars starwars-tiny.in starwars-tiny.out
	${BINDIR}/starwars starwars-tiny.in starwars-tiny.xout
	diff starwars-tiny.xout starwars-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
