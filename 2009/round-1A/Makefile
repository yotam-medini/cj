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

${BINDIR}/happy-save: obj.d/happy-save.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

happy.data: ${BINDIR}/happy-save
	$< > $@

${BINDIR}/happy: obj.d/happy.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

happy-test: ${BINDIR}/happy happy-tiny.in happy-tiny.out happy.data
	${BINDIR}/happy happy-tiny.in happy-tiny.xout
	diff happy-tiny.xout happy-tiny.out

${BINDIR}/crossing: obj.d/crossing.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

crossing-test: ${BINDIR}/crossing crossing-tiny.in crossing-tiny.out
	${BINDIR}/crossing crossing-tiny.in crossing-tiny.xout
	diff crossing-tiny.xout crossing-tiny.out

${BINDIR}/cards: obj.d/cards.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

cards-test: ${BINDIR}/cards cards-tiny.in cards-tiny.out
	${BINDIR}/cards cards-tiny.in cards-tiny.xout
	diff cards-tiny.xout cards-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR} happy.data

-include ${DEPS}
