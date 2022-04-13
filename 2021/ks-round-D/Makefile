#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/00000000004361e3

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++17 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/asquare: obj.d/asquare.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

asquare-test: ${BINDIR}/asquare asquare-tiny.in asquare-tiny.out
	timeout 2 ${BINDIR}/asquare -naive asquare-tiny.in asquare-tiny.xnout
	diff asquare-tiny.xnout asquare-tiny.out
	timeout 2 ${BINDIR}/asquare asquare-tiny.in asquare-tiny.xout
	diff asquare-tiny.xout asquare-tiny.out

${BINDIR}/cutting: obj.d/cutting.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cutting-test: ${BINDIR}/cutting cutting-tiny.in cutting-tiny.out
	timeout 2 ${BINDIR}/cutting -naive cutting-tiny.in cutting-tiny.xnout
	diff cutting-tiny.xnout cutting-tiny.out
	timeout 2 ${BINDIR}/cutting cutting-tiny.in cutting-tiny.xout
	diff cutting-tiny.xout cutting-tiny.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 2 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 2 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 2 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
