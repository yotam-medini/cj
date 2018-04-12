#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/07

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/saveuni: obj.d/saveuni.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

saveuni-test: ${BINDIR}/saveuni saveuni-tiny.in saveuni-tiny.out
	${BINDIR}/saveuni saveuni-tiny.in saveuni-tiny.xout
	diff saveuni-tiny.xout saveuni-tiny.out

${BINDIR}/trbsort: obj.d/trbsort.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

trbsort-test: ${BINDIR}/trbsort trbsort-tiny.in trbsort-tiny.out
	${BINDIR}/trbsort -naive trbsort-tiny.in trbsort-tiny.xnout
	diff trbsort-tiny.xnout trbsort-tiny.out
	${BINDIR}/trbsort trbsort-tiny.in trbsort-tiny.xout
	diff trbsort-tiny.xout trbsort-tiny.out

${BINDIR}/gogopher: obj.d/gogopher.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

gogopher-test: ${BINDIR}/gogopher gogopher-tiny.in gogopher-tiny.out
	${BINDIR}/gogopher gogopher-tiny.in gogopher-tiny.xout
	diff gogopher-tiny.xout gogopher-tiny.out

${BINDIR}/cubicufo: obj.d/cubicufo.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cubicufo-test: ${BINDIR}/cubicufo cubicufo-tiny.in cubicufo-tiny.out
	${BINDIR}/cubicufo cubicufo-tiny.in cubicufo-tiny.xout
	diff cubicufo-tiny.xout cubicufo-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
