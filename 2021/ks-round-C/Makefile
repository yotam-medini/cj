#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/0000000000435c44
ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++17 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/smaller: obj.d/smaller.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

smaller-test: ${BINDIR}/smaller smaller-tiny.in smaller-tiny.out
	timeout 2 ${BINDIR}/smaller -naive smaller-tiny.in smaller-tiny.xnout
	diff smaller-tiny.xnout smaller-tiny.out
	timeout 2 ${BINDIR}/smaller smaller-tiny.in smaller-tiny.xout
	diff smaller-tiny.xout smaller-tiny.out

${BINDIR}/alien: obj.d/alien.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

alien-test: ${BINDIR}/alien alien-tiny.in alien-tiny.out
	timeout 2 ${BINDIR}/alien -naive alien-tiny.in alien-tiny.xnout
	diff alien-tiny.xnout alien-tiny.out
	timeout 2 ${BINDIR}/alien alien-tiny.in alien-tiny.xout
	diff alien-tiny.xout alien-tiny.out

${BINDIR}/rps: rps.cc
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rps-test: ${BINDIR}/rps gentest-rps.py
	timeout 2 ${BINDIR}/rps rps-tiny.in rps-tiny.xnout
	./gentest-rps.py 1

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
