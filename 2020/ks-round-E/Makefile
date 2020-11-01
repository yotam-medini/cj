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

${BINDIR}/longarith: obj.d/longarith.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

longarith-test: ${BINDIR}/longarith longarith-tiny.in longarith-tiny.out
	timeout 8 ${BINDIR}/longarith -naive longarith-tiny.in longarith-tiny.xnout
	diff longarith-tiny.xnout longarith-tiny.out
	timeout 8 ${BINDIR}/longarith longarith-tiny.in longarith-tiny.xout
	diff longarith-tiny.xout longarith-tiny.out

${BINDIR}/highbuild: obj.d/highbuild.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

highbuild-test: ${BINDIR}/highbuild highbuild-tiny.in highbuild-tiny.out
	timeout 4 ${BINDIR}/highbuild -naive highbuild-tiny.in highbuild-tiny.xnout
	diff highbuild-tiny.xnout highbuild-tiny.out
	timeout 4 ${BINDIR}/highbuild highbuild-tiny.in highbuild-tiny.xout
	diff highbuild-tiny.xout highbuild-tiny.out

${BINDIR}/toys: obj.d/toys.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

toys-test: ${BINDIR}/toys toys-tiny.in toys-tiny.out
	timeout 4 ${BINDIR}/toys -naive toys-tiny.in toys-tiny.xnout
	diff toys-tiny.xnout toys-tiny.out
	timeout 4 ${BINDIR}/toys toys-tiny.in toys-tiny.xout
	diff toys-tiny.xout toys-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	timeout 4 ${BINDIR}/Dxxx -naive Dxxx-tiny.in Dxxx-tiny.xnout
	diff Dxxx-tiny.xnout Dxxx-tiny.out
	timeout 4 ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
