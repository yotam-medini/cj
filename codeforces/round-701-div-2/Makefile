#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 

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

${BINDIR}/adiv: obj.d/adiv.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

adiv-test: ${BINDIR}/adiv adiv-tiny.in adiv-tiny.out
	timeout 4 ${BINDIR}/adiv -naive adiv-tiny.in adiv-tiny.xnout
	diff adiv-tiny.xnout adiv-tiny.out
	timeout 4 ${BINDIR}/adiv adiv-tiny.in adiv-tiny.xout
	diff adiv-tiny.xout adiv-tiny.out

${BINDIR}/repkeep: obj.d/repkeep.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

repkeep-test: ${BINDIR}/repkeep repkeep-tiny.in repkeep-tiny.out
	timeout 4 ${BINDIR}/repkeep -naive repkeep-tiny.in repkeep-tiny.xnout
	diff repkeep-tiny.xnout repkeep-tiny.out
	timeout 4 ${BINDIR}/repkeep repkeep-tiny.in repkeep-tiny.xout
	diff repkeep-tiny.xout repkeep-tiny.out
	timeout 4 ${BINDIR}/repkeep -naive repkeep-tiny2.in repkeep-tiny2.xnout
	diff repkeep-tiny2.xnout repkeep-tiny2.out
	timeout 4 ${BINDIR}/repkeep repkeep-tiny2.in repkeep-tiny2.xout
	diff repkeep-tiny2.xout repkeep-tiny2.out

${BINDIR}/Cxxx: obj.d/Cxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Cxxx-test: ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.out
	timeout 4 ${BINDIR}/Cxxx -naive Cxxx-tiny.in Cxxx-tiny.xnout
	diff Cxxx-tiny.xnout Cxxx-tiny.out
	timeout 4 ${BINDIR}/Cxxx Cxxx-tiny.in Cxxx-tiny.xout
	diff Cxxx-tiny.xout Cxxx-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 4 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 4 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

${BINDIR}/Exxx: obj.d/Exxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Exxx-test: ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.out
	timeout 4 ${BINDIR}/Exxx -naive Exxx-tiny.in Exxx-tiny.xnout
	diff Exxx-tiny.xnout Exxx-tiny.out
	timeout 4 ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.xout
	diff Exxx-tiny.xout Exxx-tiny.out

${BINDIR}/Fxxx: obj.d/Fxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Fxxx-test: ${BINDIR}/Fxxx Fxxx-tiny.in Fxxx-tiny.out
	timeout 4 ${BINDIR}/Fxxx -naive Fxxx-tiny.in Fxxx-tiny.xnout
	diff Fxxx-tiny.xnout Fxxx-tiny.out
	timeout 4 ${BINDIR}/Fxxx Fxxx-tiny.in Fxxx-tiny.xout
	diff Fxxx-tiny.xout Fxxx-tiny.out

${BINDIR}/Gxxx: obj.d/Gxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Gxxx-test: ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.out
	timeout 4 ${BINDIR}/Gxxx -naive Gxxx-tiny.in Gxxx-tiny.xnout
	diff Gxxx-tiny.xnout Gxxx-tiny.out
	timeout 4 ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.xout
	diff Gxxx-tiny.xout Gxxx-tiny.out

${BINDIR}/Hxxx: obj.d/Hxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Hxxx-test: ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.out
	timeout 4 ${BINDIR}/Hxxx -naive Hxxx-tiny.in Hxxx-tiny.xnout
	diff Hxxx-tiny.xnout Hxxx-tiny.out
	timeout 4 ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.xout
	diff Hxxx-tiny.xout Hxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
