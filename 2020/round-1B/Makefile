#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000019fef2

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

${BINDIR}/expogo: obj.d/expogo.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

expogo-test: ${BINDIR}/expogo expogo-tiny.in expogo-tiny.out
	timeout 2 ${BINDIR}/expogo -naive expogo-tiny.in expogo-tiny.xnout
	diff expogo-tiny.xnout expogo-tiny.out
	timeout 2 ${BINDIR}/expogo expogo-tiny.in expogo-tiny.xout
	diff expogo-tiny.xout expogo-tiny.out

${BINDIR}/blind: obj.d/blind.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

blind-test-0: ${BINDIR}/blind interactive_runner.py testing_tool.py
	timeout 4 python interactive_runner.py \
	  python ./testing_tool.py 0 -- ./${BINDIR}/blind -debug 0x1

blind-test: blind-test-0

${BINDIR}/ranks: obj.d/ranks.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ranks-test: ${BINDIR}/ranks ranks-tiny.in ranks-tiny.out
	timeout 8 ${BINDIR}/ranks -naive ranks-tiny.in ranks-tiny.xnout
	diff ranks-tiny.xnout ranks-tiny.out
	timeout 8 ${BINDIR}/ranks ranks-tiny.in ranks-tiny.xout
	diff ranks-tiny.xout ranks-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
