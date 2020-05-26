#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000019fd74

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

${BINDIR}/pattern: obj.d/pattern.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pattern-test: ${BINDIR}/pattern pattern-tiny.in pattern-tiny.out
	timeout 8 ${BINDIR}/pattern -naive pattern-tiny.in pattern-tiny.xnout
	diff pattern-tiny.xnout pattern-tiny.out
	timeout 8 ${BINDIR}/pattern pattern-tiny.in pattern-tiny.xout
	diff pattern-tiny.xout pattern-tiny.out

${BINDIR}/pascalwalk: obj.d/pascalwalk.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pascalwalk-test: ${BINDIR}/pascalwalk pascalwalk-tiny.in pascalwalk-tiny.out
	timeout 4 ${BINDIR}/pascalwalk -naive pascalwalk-tiny.in pascalwalk-tiny.xnout
	timeout 4 ${BINDIR}/pascalwalk pascalwalk-tiny.in pascalwalk-tiny.xout

${BINDIR}/sqdance: obj.d/sqdance.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

sqdance-test: ${BINDIR}/sqdance sqdance-tiny.in sqdance-tiny.out
	timeout 2 ${BINDIR}/sqdance -naive sqdance-tiny.in sqdance-tiny.xnout
	diff sqdance-tiny.xnout sqdance-tiny.out
	timeout 2 ${BINDIR}/sqdance sqdance-tiny.in sqdance-tiny.xout
	diff sqdance-tiny.xout sqdance-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
