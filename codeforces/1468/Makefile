#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# https://codeforces.com/contest/1468

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
DEFLAGS =
CFLAGS = ${DEFLAGS} ${OPTFLAGS} -Wall -Wshadow -std=c++14 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/lais: obj.d/lais.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lais-test: ${BINDIR}/lais lais-tiny.in lais-tiny.out
	timeout 4 ${BINDIR}/lais -naive lais-tiny.in lais-tiny.xnout
	diff lais-tiny.xnout lais-tiny.out
	timeout 4 ${BINDIR}/lais lais-tiny.in lais-tiny.xout
	diff lais-tiny.xout lais-tiny.out

lais-ptest: ${BINDIR}/lais
	timeout 4 ${BINDIR}/lais test_permutations

${BINDIR}/Bxxx: obj.d/Bxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Bxxx-test: ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.out
	timeout 4 ${BINDIR}/Bxxx -naive Bxxx-tiny.in Bxxx-tiny.xnout
	diff Bxxx-tiny.xnout Bxxx-tiny.out
	timeout 4 ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.xout
	diff Bxxx-tiny.xout Bxxx-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 4 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 4 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

# D
${BINDIR}/firecrack: obj.d/firecrack.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

firecrack-test: ${BINDIR}/firecrack firecrack-tiny.in firecrack-tiny.out
	timeout 4 ${BINDIR}/firecrack -naive firecrack-tiny.in firecrack-tiny.xnout
	diff firecrack-tiny.xnout firecrack-tiny.out
	timeout 4 ${BINDIR}/firecrack firecrack-tiny.in firecrack-tiny.xout
	diff firecrack-tiny.xout firecrack-tiny.out

${BINDIR}/similar: obj.d/similar.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

similar-test: ${BINDIR}/similar similar-tiny.in similar-tiny.out
	timeout 4 ${BINDIR}/similar -naive similar-tiny.in similar-tiny.xnout
	diff similar-tiny.xnout similar-tiny.out
	timeout 4 ${BINDIR}/similar similar-tiny.in similar-tiny.xout
	diff similar-tiny.xout similar-tiny.out

${BINDIR}/waste: obj.d/waste.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

waste-test: ${BINDIR}/waste waste-tiny.in waste-tiny.out
	timeout 4 ${BINDIR}/waste -naive waste-tiny.in waste-tiny.xnout
	diff waste-tiny.xnout waste-tiny.out
	timeout 4 ${BINDIR}/waste waste-tiny.in waste-tiny.xout
	diff waste-tiny.xout waste-tiny.out

clean:
	rm -rf ${OBJS} ${BINDIR}

-include ${DEPS}
