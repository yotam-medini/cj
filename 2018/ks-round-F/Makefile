#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050e07
# code.google.com/codejam/contest/3314486/dashboard

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

${BINDIR}/canag: obj.d/canag.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

canag-test: ${BINDIR}/canag canag-tiny.in canag-tiny.out
	timeout 8 ${BINDIR}/canag -naive canag-tiny.in canag-tiny.xnout
	diff canag-tiny.xnout canag-tiny.out
	timeout 8 ${BINDIR}/canag canag-tiny.in canag-tiny.xout
	diff canag-tiny.xout canag-tiny.out

${BINDIR}/specvill: obj.d/specvill.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

specvill-test: ${BINDIR}/specvill specvill-tiny.in specvill-tiny.out
	timeout 8 ${BINDIR}/specvill -naive specvill-tiny.in specvill-tiny.xnout
	diff specvill-tiny.xnout specvill-tiny.out
	timeout 8 ${BINDIR}/specvill specvill-tiny.in specvill-tiny.xout
	diff specvill-tiny.xout specvill-tiny.out

${BINDIR}/palinseq: obj.d/palinseq.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

palinseq-test: ${BINDIR}/palinseq palinseq-tiny.in palinseq-tiny.out
	timeout 8 ${BINDIR}/palinseq -naive palinseq-tiny.in palinseq-tiny.xnout
	diff palinseq-tiny.xnout palinseq-tiny.out
	timeout 8 ${BINDIR}/palinseq palinseq-tiny.in palinseq-tiny.xout
	diff palinseq-tiny.xout palinseq-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
