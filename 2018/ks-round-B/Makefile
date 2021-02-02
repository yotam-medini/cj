#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050ff4
# code.google.com/codejam/contest/10284486/dashboard

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

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

${BINDIR}/no9: obj.d/no9.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

no9-test: ${BINDIR}/no9 no9-tiny.in no9-tiny.out
	timeout 8 ${BINDIR}/no9 -naive no9-tiny.in no9-tiny.xnout
	diff no9-tiny.xnout no9-tiny.out
	timeout 8 ${BINDIR}/no9 no9-tiny.in no9-tiny.xout
	diff no9-tiny.xout no9-tiny.out

${BINDIR}/sherbit: obj.d/sherbit.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

sherbit-test: ${BINDIR}/sherbit sherbit-tiny.in sherbit-tiny.out
	timeout 8 ${BINDIR}/sherbit -naive sherbit-tiny.in sherbit-tiny.xnout
	diff sherbit-tiny.xnout sherbit-tiny.out
	timeout 8 ${BINDIR}/sherbit sherbit-tiny.in sherbit-tiny.xout
	diff sherbit-tiny.xout sherbit-tiny.out

${BINDIR}/king: obj.d/king.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

king-test: ${BINDIR}/king king-tiny.in king-tiny.out
	timeout 8 ${BINDIR}/king -naive king-tiny.in king-tiny.xnout
	diff king-tiny.xnout king-tiny.out
	timeout 8 ${BINDIR}/king king-tiny.in king-tiny.xout
	diff king-tiny.xout king-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
