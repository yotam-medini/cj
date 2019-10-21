#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050edc

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu

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

${BINDIR}/flatten: obj.d/flatten.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

flatten-test: ${BINDIR}/flatten flatten-tiny.in flatten-tiny.out
	timeout 8 ${BINDIR}/flatten -naive flatten-tiny.in flatten-tiny.xnout
	diff flatten-tiny.xnout flatten-tiny.out
	timeout 8 ${BINDIR}/flatten flatten-tiny.in flatten-tiny.xout
	diff flatten-tiny.xout flatten-tiny.out

${BINDIR}/teachme: obj.d/teachme.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

teachme-test: ${BINDIR}/teachme teachme-tiny.in teachme-tiny.out
	timeout 4 ${BINDIR}/teachme -naive teachme-tiny.in teachme-tiny.xnout
	diff teachme-tiny.xnout teachme-tiny.out
	timeout 4 ${BINDIR}/teachme teachme-tiny.in teachme-tiny.xout
	diff teachme-tiny.xout teachme-tiny.out

${BINDIR}/villages: obj.d/villages.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

villages-test: ${BINDIR}/villages villages-tiny.in villages-tiny.out
	timeout 8 ${BINDIR}/villages -naive villages-tiny.in villages-tiny.xnout
	diff villages-tiny.xnout villages-tiny.out
	timeout 8 ${BINDIR}/villages villages-tiny.in villages-tiny.xout
	diff villages-tiny.xout villages-tiny.out


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
