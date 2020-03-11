#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050e07

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

${BINDIR}/Bxxx: obj.d/Bxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Bxxx-test: ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx -naive Bxxx-tiny.in Bxxx-tiny.xnout
	diff Bxxx-tiny.xnout Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.xout
	diff Bxxx-tiny.xout Bxxx-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 8 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 8 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
