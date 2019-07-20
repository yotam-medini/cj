#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/May/04
https://codingcompetitions.withgoogle.com/codejam/round/00000000000516b9/

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

${BINDIR}/robot: obj.d/robot.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

robot-test: ${BINDIR}/robot robot-tiny.in robot-tiny.out
	timeout 8 ${BINDIR}/robot -naive robot-tiny.in robot-tiny.xnout
	diff robot-tiny.xnout robot-tiny.out
	timeout 8 ${BINDIR}/robot robot-tiny.in robot-tiny.xout
	diff robot-tiny.xout robot-tiny.out

${BINDIR}/arranger: obj.d/arranger.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

PYRUNPY = timeout 4 python ./interactive_runner.py python
arranger-test: ${BINDIR}/arranger
	${PYRUNPY} testing_tool.py 0 -- $<
	${PYRUNPY} testing_tool.py 1 -- $<

${BINDIR}/bacterial: obj.d/bacterial.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bacterial-test: ${BINDIR}/bacterial bacterial-tiny.in bacterial-tiny.out
	timeout 8 ${BINDIR}/bacterial -naive bacterial-tiny.in bacterial-tiny.xnout
	diff bacterial-tiny.xnout bacterial-tiny.out
	timeout 8 ${BINDIR}/bacterial bacterial-tiny.in bacterial-tiny.xout
	diff bacterial-tiny.xout bacterial-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
