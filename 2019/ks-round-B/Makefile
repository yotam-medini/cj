#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/codejam/round/0000000000051706

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

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

${BINDIR}/palin: obj.d/palin.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

palin-test: ${BINDIR}/palin palin-tiny.in palin-tiny.out
	timeout 8 ${BINDIR}/palin -naive palin-tiny.in palin-tiny.xnout
	diff palin-tiny.xnout palin-tiny.out
	timeout 8 ${BINDIR}/palin palin-tiny.in palin-tiny.xout
	diff palin-tiny.xout palin-tiny.out

${BINDIR}/energy: obj.d/energy.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

energy-test: ${BINDIR}/energy energy-tiny.in energy-tiny.out
	timeout 8 ${BINDIR}/energy -naive energy-tiny.in energy-tiny.xnout
	diff energy-tiny.xnout energy-tiny.out
	timeout 8 ${BINDIR}/energy energy-tiny.in energy-tiny.xout
	diff energy-tiny.xout energy-tiny.out

${BINDIR}/diverse: obj.d/diverse.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

diverse-test: ${BINDIR}/diverse diverse-tiny.in diverse-tiny.out
	timeout 8 ${BINDIR}/diverse -naive diverse-tiny.in diverse-tiny.xnout
	diff diverse-tiny.xnout diverse-tiny.out
	timeout 8 ${BINDIR}/diverse diverse-tiny.in diverse-tiny.xout
	diff diverse-tiny.xout diverse-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
