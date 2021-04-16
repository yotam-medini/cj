#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000043580a

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

${BINDIR}/revsort: obj.d/revsort.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

revsort-test: ${BINDIR}/revsort revsort-tiny.in revsort-tiny.out
	timeout 2 ${BINDIR}/revsort -naive revsort-tiny.in revsort-tiny.xnout
	diff revsort-tiny.xnout revsort-tiny.out
	timeout 2 ${BINDIR}/revsort revsort-tiny.in revsort-tiny.xout
	diff revsort-tiny.xout revsort-tiny.out

${BINDIR}/moons: obj.d/moons.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

moons-test: ${BINDIR}/moons moons-tiny.in moons-tiny.out
	timeout 2 ${BINDIR}/moons -naive moons-tiny.in moons-tiny.xnout
	diff moons-tiny.xnout moons-tiny.out
	timeout 2 ${BINDIR}/moons moons-tiny.in moons-tiny.xout
	diff moons-tiny.xout moons-tiny.out
	timeout 2 ${BINDIR}/moons -naive moons-tiny3.in moons-tiny3.xnout
	diff moons-tiny3.xnout moons-tiny3.out
	timeout 2 ${BINDIR}/moons moons-tiny3.in moons-tiny3.xout
	diff moons-tiny3.xout moons-tiny3.out

${BINDIR}/reveng: obj.d/reveng.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

reveng-test: ${BINDIR}/reveng reveng-tiny.in reveng-tiny.out
	timeout 2 ${BINDIR}/reveng -naive reveng-tiny.in reveng-tiny.xnout
	diff <(grep IMPOSSIBLE reveng-tiny.xnout) <(grep IMPOSSIBLE reveng-tiny.out)
	timeout 2 ${BINDIR}/reveng reveng-tiny.in reveng-tiny.xout
	diff <(grep IMPOSSIBLE reveng-tiny.xout) <(grep IMPOSSIBLE reveng-tiny.out)

${BINDIR}/mediansort: obj.d/mediansort.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

MEDIANSORT_DEBUG=0x1
mediansort-test: \
	${BINDIR}/mediansort interactive_runner.py my_testing_tool.py \
	mediansort-test-0 \
	mediansort-test-1 \
	mediansort-test-2

mediansort-test-0:
	timeout 4 python interactive_runner.py \
	  python3 my_testing_tool.py 0 -- \
	  ./${BINDIR}/mediansort -debug ${MEDIANSORT_DEBUG}

mediansort-test-1:
	timeout 60 python interactive_runner.py \
	  python3 my_testing_tool.py 1 -- \
	  ./${BINDIR}/mediansort -debug ${MEDIANSORT_DEBUG}

mediansort-test-2:
	timeout 60 python interactive_runner.py \
	  python3 my_testing_tool.py 2 -- \
	  ./${BINDIR}/mediansort -debug ${MEDIANSORT_DEBUG}

${BINDIR}/cheating: obj.d/cheating.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cheating-test: ${BINDIR}/cheating cheating-tiny.in cheating-tiny.out
	timeout 2 ${BINDIR}/cheating cheating-tiny.in cheating-tiny.xout
	diff cheating-tiny.xout cheating-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
