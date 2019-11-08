#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050edf

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

${BINDIR}/evendigits: obj.d/evendigits.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

evendigits-test: ${BINDIR}/evendigits evendigits-tiny.in evendigits-tiny.out
	timeout 8 ${BINDIR}/evendigits -naive evendigits-tiny.in evendigits-tiny.xnout
	diff evendigits-tiny.xnout evendigits-tiny.out
	timeout 8 ${BINDIR}/evendigits evendigits-tiny.in evendigits-tiny.xout
	diff evendigits-tiny.xout evendigits-tiny.out

${BINDIR}/luckydip: obj.d/luckydip.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

luckydip-test: ${BINDIR}/luckydip luckydip-tiny.in luckydip-tiny.out
	timeout 8 ${BINDIR}/luckydip -naive luckydip-tiny.in luckydip-tiny.xnout
	diff luckydip-tiny.xnout luckydip-tiny.out
	timeout 8 ${BINDIR}/luckydip luckydip-tiny.in luckydip-tiny.xout
	diff luckydip-tiny.xout luckydip-tiny.out

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
