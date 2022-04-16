#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/00000000004361e3

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

${BINDIR}/asquare: obj.d/asquare.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

asquare-test: ${BINDIR}/asquare asquare-tiny.in asquare-tiny.out
	timeout 2 ${BINDIR}/asquare -naive asquare-tiny.in asquare-tiny.xnout
	diff asquare-tiny.xnout asquare-tiny.out
	timeout 2 ${BINDIR}/asquare asquare-tiny.in asquare-tiny.xout
	diff asquare-tiny.xout asquare-tiny.out

${BINDIR}/cutting: obj.d/cutting.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cutting-test: ${BINDIR}/cutting cutting-tiny.in cutting-tiny.out
	timeout 2 ${BINDIR}/cutting -naive cutting-tiny.in cutting-tiny.xnout
	diff cutting-tiny.xnout cutting-tiny.out
	timeout 2 ${BINDIR}/cutting cutting-tiny.in cutting-tiny.xout
	diff cutting-tiny.xout cutting-tiny.out

${BINDIR}/exam: obj.d/exam.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

exam-test: ${BINDIR}/exam exam-tiny.in exam-tiny.out
	timeout 2 ${BINDIR}/exam -naive exam-tiny.in exam-tiny.xnout
	diff exam-tiny.xnout exam-tiny.out
	timeout 2 ${BINDIR}/exam exam-tiny.in exam-tiny.xout
	diff exam-tiny.xout exam-tiny.out

exam-internal-test: ${BINDIR}/exam
	./bin/exam test 42 10000 10000 10000 10000 \
	    1000000000000000000 1000000000000000000

${BINDIR}/pqueries: obj.d/pqueries.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pqueries-test: ${BINDIR}/pqueries pqueries-tiny.in pqueries-tiny.out
	timeout 2 ${BINDIR}/pqueries -naive pqueries-tiny.in pqueries-tiny.xnout
	diff pqueries-tiny.xnout pqueries-tiny.out
	timeout 2 ${BINDIR}/pqueries pqueries-tiny.in pqueries-tiny.xout
	diff pqueries-tiny.xout pqueries-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
