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
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -fno-exceptions -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin
TL = timelimit -t 6

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/crazyrows: obj.d/crazyrows.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

crazyrows-test: ${BINDIR}/crazyrows crazyrows-tiny.in crazyrows-tiny.out
	${BINDIR}/crazyrows -naive crazyrows-tiny.in crazyrows-tiny.xnout
	diff crazyrows-tiny.xnout crazyrows-tiny.out
	${BINDIR}/crazyrows crazyrows-tiny.in crazyrows-tiny.xout
	diff crazyrows-tiny.xout crazyrows-tiny.out

${BINDIR}/digging: obj.d/digging.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

digging-test: ${BINDIR}/digging digging-tiny.in digging-tiny.out
	timelimit -t 6 ${BINDIR}/digging digging-tiny.in digging-tiny.xout
	diff digging-tiny.xout digging-tiny.out

STOCKCHART_OBJS = \
	obj.d/stockchart.o \
	obj.d/bipartite_match.o \
	obj.d/maxflow.o

${BINDIR}/stockchart: ${STOCKCHART_OBJS}
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

stockchart-test: ${BINDIR}/stockchart stockchart-tiny.in stockchart-tiny.out
	${TL} ${BINDIR}/stockchart -naive \
	    stockchart-tiny.in stockchart-tiny.xnout
	diff stockchart-tiny.xnout stockchart-tiny.out
	${TL} ${BINDIR}/stockchart stockchart-tiny.in stockchart-tiny.xout
	diff stockchart-tiny.xout stockchart-tiny.out

OBJS_watering = \
	obj.d/tricirc.o \
	obj.d/watering.o

${BINDIR}/watering: ${OBJS_watering}
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

watering-test: ${BINDIR}/watering watering-tiny.in watering-tiny.out
	${BINDIR}/watering watering-tiny.in watering-tiny.xout
	diff watering-tiny.xout watering-tiny.out

/tmp/tricirc: tricirc.cc
	g++ ${CFLAGS} -DTEST=1 -o $@ $^

tricirc-test: /tmp/tricirc
	/tmp/tricirc 0 0 1  1 1 1  2 0 1



clean:
	rm -f ${OBJS} ${BINDIR}


-include ${DEPS}
