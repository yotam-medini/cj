#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/00000000008cb33e

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

${BINDIR}/speed: obj.d/speed.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

speed-test: ${BINDIR}/speed speed-tiny.in speed-tiny.out \
	    speed-tiny2.in speed-tiny2.out
	timeout 2 ${BINDIR}/speed -naive speed-tiny.in speed-tiny.xnout
	diff speed-tiny.xnout speed-tiny.out
	timeout 2 ${BINDIR}/speed speed-tiny.in speed-tiny.xout
	diff speed-tiny.xout speed-tiny.out
	timeout 2 ${BINDIR}/speed -naive speed-tiny2.in speed-tiny2.xnout
	diff speed-tiny2.xnout speed-tiny2.out
	timeout 2 ${BINDIR}/speed speed-tiny2.in speed-tiny2.xout
	diff speed-tiny2.xout speed-tiny2.out

${BINDIR}/nine: obj.d/nine.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

nine-test: ${BINDIR}/nine nine-tiny.in nine-tiny.out
	timeout 2 ${BINDIR}/nine -naive nine-tiny.in nine-tiny.xnout
	diff nine-tiny.xnout nine-tiny.out
	timeout 2 ${BINDIR}/nine nine-tiny.in nine-tiny.xout
	diff nine-tiny.xout nine-tiny.out

${BINDIR}/palifree: obj.d/palifree.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

palifree-test: ${BINDIR}/palifree palifree-tiny.in palifree-tiny.out
	timeout 2 ${BINDIR}/palifree -naive palifree-tiny.in palifree-tiny.xnout
	diff palifree-tiny.xnout palifree-tiny.out
	timeout 2 ${BINDIR}/palifree palifree-tiny.in palifree-tiny.xout
	diff palifree-tiny.xout palifree-tiny.out

${BINDIR}/ints: obj.d/ints.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ints-test: ${BINDIR}/ints ints-tiny.in ints-tiny.out
	timeout 2 ${BINDIR}/ints -naive ints-tiny.in ints-tiny.xnout
	diff ints-tiny.xnout ints-tiny.out
	timeout 2 ${BINDIR}/ints ints-tiny.in ints-tiny.xout
	diff ints-tiny.xout ints-tiny.out
	timeout 2 ${BINDIR}/ints -method2 ints-tiny.in ints-tiny.x2out
	diff ints-tiny.x2out ints-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
