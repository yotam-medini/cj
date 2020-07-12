#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ff08

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
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

${BINDIR}/record: obj.d/record.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

record-test: ${BINDIR}/record record-tiny.in record-tiny.out
	timeout 8 ${BINDIR}/record -naive record-tiny.in record-tiny.xnout
	diff record-tiny.xnout record-tiny.out
	timeout 8 ${BINDIR}/record record-tiny.in record-tiny.xout
	diff record-tiny.xout record-tiny.out

${BINDIR}/piano: obj.d/piano.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

piano-test: ${BINDIR}/piano piano-tiny.in piano-tiny.out
	timeout 8 ${BINDIR}/piano -naive piano-tiny.in piano-tiny.xnout
	diff piano-tiny.xnout piano-tiny.out
	timeout 8 ${BINDIR}/piano piano-tiny.in piano-tiny.xout
	diff piano-tiny.xout piano-tiny.out

${BINDIR}/beautree: obj.d/beautree.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

beautree-test: ${BINDIR}/beautree beautree-tiny.in beautree-tiny.out
	timeout 8 ${BINDIR}/beautree -naive beautree-tiny.in beautree-tiny.xnout
	numdiff -r 0.000001 beautree-tiny.xnout beautree-tiny.out
	timeout 8 ${BINDIR}/beautree beautree-tiny.in beautree-tiny.xout
	numdiff -r 0.000001 beautree-tiny.xout beautree-tiny.out

${BINDIR}/doors: obj.d/doors.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

doors-test: ${BINDIR}/doors doors-tiny.in doors-tiny.out
	timeout 8 ${BINDIR}/doors -naive doors-tiny.in doors-tiny.xnout
	diff doors-tiny.xnout doors-tiny.out
	timeout 8 ${BINDIR}/doors doors-tiny.in doors-tiny.xout
	diff doors-tiny.xout doors-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
