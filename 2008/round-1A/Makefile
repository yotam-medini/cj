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
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -Wunused -Werror -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/minscalar: obj.d/minscalar.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ $(LIBGMP)

minscalar-test: ${BINDIR}/minscalar minscalar-tiny.in minscalar-tiny.out
	${BINDIR}/minscalar minscalar-tiny.in minscalar-tiny.xout
	diff minscalar-tiny.xout minscalar-tiny.out

${BINDIR}/milkshake: obj.d/milkshake.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

milkshake-test: ${BINDIR}/milkshake milkshake-tiny.in milkshake-tiny.out
	${BINDIR}/milkshake milkshake-tiny.in milkshake-tiny.xout
	diff milkshake-tiny.xout milkshake-tiny.out

${BINDIR}/3sq5: obj.d/3sq5.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ $(LIBGMP)

3sq5-test: ${BINDIR}/3sq5 3sq5-tiny.in 3sq5-tiny.out
	${BINDIR}/3sq5 3sq5-tiny.in 3sq5-tiny.xout
	diff 3sq5-tiny.xout 3sq5-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
