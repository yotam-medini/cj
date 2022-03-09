#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000043585d

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

${BINDIR}/asort: obj.d/asort.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

asort-test: ${BINDIR}/asort asort-tiny.in asort-tiny.out
	timeout 2 ${BINDIR}/asort -naive asort-tiny.in asort-tiny.xnout
	diff asort-tiny.xnout asort-tiny.out
	timeout 2 ${BINDIR}/asort asort-tiny.in asort-tiny.xout
	diff asort-tiny.xout asort-tiny.out

${BINDIR}/prime: obj.d/prime.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

prime-test: ${BINDIR}/prime prime-tiny.in prime-tiny.out
	timeout 2 ${BINDIR}/prime -naive prime-tiny.in prime-tiny.xnout
	diff prime-tiny.xnout prime-tiny.out
	timeout 2 ${BINDIR}/prime prime-tiny.in prime-tiny.xout
	diff prime-tiny.xout prime-tiny.out

${BINDIR}/exam: obj.d/exam.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

exam-test: ${BINDIR}/exam exam-tiny.in exam-tiny.out
	timeout 2 ${BINDIR}/exam -naive exam-tiny.in exam-tiny.xnout
	diff <(sed -e s/[FT]/X/g < exam-tiny.xnout) <(sed -e s/[FT]/X/g < exam-tiny.out)
	timeout 2 ${BINDIR}/exam exam-tiny.in exam-tiny.xout
	diff <(sed -e s/[FT]/X/g < exam-tiny.xout) <(sed -e s/[FT]/X/g < exam-tiny.out)

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
