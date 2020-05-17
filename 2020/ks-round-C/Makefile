#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ff43

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
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

${BINDIR}/cdown: obj.d/cdown.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cdown-test: ${BINDIR}/cdown cdown-tiny.in cdown-tiny.out
	timeout 8 ${BINDIR}/cdown -naive cdown-tiny.in cdown-tiny.xnout
	diff cdown-tiny.xnout cdown-tiny.out
	timeout 8 ${BINDIR}/cdown cdown-tiny.in cdown-tiny.xout
	diff cdown-tiny.xout cdown-tiny.out

${BINDIR}/stablewall: obj.d/stablewall.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

stablewall-test: ${BINDIR}/stablewall stablewall-tiny.in stablewall-tiny.out
	timeout 2 ${BINDIR}/stablewall -naive stablewall-tiny.in stablewall-tiny.xnout
	diff stablewall-tiny.xnout stablewall-tiny.out
	timeout 2 ${BINDIR}/stablewall stablewall-tiny.in stablewall-tiny.xout
	diff stablewall-tiny.xout stablewall-tiny.out

${BINDIR}/perfsub: obj.d/perfsub.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

perfsub-test: ${BINDIR}/perfsub perfsub-tiny.in perfsub-tiny.out
	timeout 8 ${BINDIR}/perfsub -naive perfsub-tiny.in perfsub-tiny.xnout
	diff perfsub-tiny.xnout perfsub-tiny.out
	timeout 8 ${BINDIR}/perfsub perfsub-tiny.in perfsub-tiny.xout
	diff perfsub-tiny.xout perfsub-tiny.out

${BINDIR}/candies: obj.d/candies.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

candies-test: ${BINDIR}/candies candies-tiny.in candies-tiny.out
	timeout 8 ${BINDIR}/candies -naive candies-tiny.in candies-tiny.xnout
	diff candies-tiny.xnout candies-tiny.out
	timeout 8 ${BINDIR}/candies candies-tiny.in candies-tiny.xout
	diff candies-tiny.xout candies-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
