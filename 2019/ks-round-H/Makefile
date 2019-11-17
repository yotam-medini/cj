#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050edd

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

${BINDIR}/hindex: obj.d/hindex.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

hindex-test: ${BINDIR}/hindex hindex-tiny.in hindex-tiny.out
	timeout 8 ${BINDIR}/hindex -naive hindex-tiny.in hindex-tiny.xnout
	diff hindex-tiny.xnout hindex-tiny.out
	timeout 8 ${BINDIR}/hindex hindex-tiny.in hindex-tiny.xout
	diff hindex-tiny.xout hindex-tiny.out

${BINDIR}/diagpuz: obj.d/diagpuz.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

diagpuz-test: ${BINDIR}/diagpuz diagpuz-tiny.in diagpuz-tiny.out
	timeout 8 ${BINDIR}/diagpuz -naive diagpuz-tiny.in diagpuz-tiny.xnout
	diff diagpuz-tiny.xnout diagpuz-tiny.out
	timeout 8 ${BINDIR}/diagpuz diagpuz-tiny.in diagpuz-tiny.xout
	diff diagpuz-tiny.xout diagpuz-tiny.out

${BINDIR}/elevengram: obj.d/elevengram.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

elevengram-test: ${BINDIR}/elevengram elevengram-tiny.in elevengram-tiny.out
	timeout 8 ${BINDIR}/elevengram -naive elevengram-tiny.in elevengram-tiny.xnout
	diff elevengram-tiny.xnout elevengram-tiny.out
	timeout 8 ${BINDIR}/elevengram elevengram-tiny.in elevengram-tiny.xout
	diff elevengram-tiny.xout elevengram-tiny.out


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
