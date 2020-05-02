#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 

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

${BINDIR}/excite: obj.d/excite.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

excite-test: ${BINDIR}/excite excite-tiny.in excite-tiny.out
	timeout 8 ${BINDIR}/excite -naive excite-tiny.in excite-tiny.xnout
	diff excite-tiny.xnout excite-tiny.out
	timeout 8 ${BINDIR}/excite excite-tiny.in excite-tiny.xout
	diff excite-tiny.xout excite-tiny.out
	timeout 8 ${BINDIR}/excite -naive excite-tiny2.in excite-tiny2.xnout
	diff excite-tiny2.xnout excite-tiny2.out
	timeout 8 ${BINDIR}/excite excite-tiny2.in excite-tiny2.xout
	diff excite-tiny2.xout excite-tiny2.out

${BINDIR}/overrand: obj.d/overrand.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

overrand-test1: ${BINDIR}/overrand sample.in.txt sample.out.txt
	timeout 8 ${BINDIR}/overrand -naive sample.in.txt overrand-sample.xnout
	diff overrand-sample.xnout sample.out.txt
	timeout 8 ${BINDIR}/overrand sample.in.txt overrand-sample.xout
	diff overrand-sample.xout sample.out.txt

overrand-big.in: overrand-genbig.py
	overrand-genbig.py > $@

clean-overrand-big.in:
	rm -f overrand-big.in

overrand-test2: ${BINDIR}/overrand overrand-big.in
	timeout 8 ${BINDIR}/overrand -naive overrand-big.in overrand-big.xnout
	grep ABCDEFGHI overrand-big.xnout
	timeout 8 ${BINDIR}/overrand overrand-big.in overrand-big.xout
	grep ABCDEFGHI overrand-big.xout

overrand-test: overrand-test1 overrand-test2

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
