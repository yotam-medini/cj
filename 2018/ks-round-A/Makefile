#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050edf

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

${BINDIR}/evendigits: obj.d/evendigits.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

evendigits-test: ${BINDIR}/evendigits evendigits-tiny.in evendigits-tiny.out
	timeout 8 ${BINDIR}/evendigits -naive evendigits-tiny.in evendigits-tiny.xnout
	diff evendigits-tiny.xnout evendigits-tiny.out
	timeout 8 ${BINDIR}/evendigits evendigits-tiny.in evendigits-tiny.xout
	diff evendigits-tiny.xout evendigits-tiny.out

${BINDIR}/lucky: obj.d/lucky.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lucky-test: ${BINDIR}/lucky lucky-tiny.in lucky-tiny.out
	timeout 8 ${BINDIR}/lucky -naive lucky-tiny.in lucky-tiny.xnout
	diff lucky-tiny.xnout lucky-tiny.out
	timeout 8 ${BINDIR}/lucky lucky-tiny.in lucky-tiny.xout
	diff lucky-tiny.xout lucky-tiny.out

${BINDIR}/scrambled: obj.d/scrambled.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

scrambled-test: ${BINDIR}/scrambled scrambled-tiny.in scrambled-tiny.out
	timeout 8 ${BINDIR}/scrambled -naive scrambled-tiny.in scrambled-tiny.xnout
	diff scrambled-tiny.xnout scrambled-tiny.out
	timeout 8 ${BINDIR}/scrambled scrambled-tiny.in scrambled-tiny.xout
	diff scrambled-tiny.xout scrambled-tiny.out


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
