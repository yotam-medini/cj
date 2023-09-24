#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 

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

${BINDIR}/arigged: obj.d/arigged.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

arigged-test: ${BINDIR}/arigged arigged-tiny.in arigged-tiny.out
	timeout 8 ${BINDIR}/arigged -naive arigged-tiny.in arigged-tiny.xnout
	diff arigged-tiny.xnout arigged-tiny.out
	timeout 8 ${BINDIR}/arigged arigged-tiny.in arigged-tiny.xout
	diff arigged-tiny.xout arigged-tiny.out

${BINDIR}/Bxxx: obj.d/Bxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Bxxx-test: ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx -naive Bxxx-tiny.in Bxxx-tiny.xnout
	diff Bxxx-tiny.xnout Bxxx-tiny.out
	timeout 8 ${BINDIR}/Bxxx Bxxx-tiny.in Bxxx-tiny.xout
	diff Bxxx-tiny.xout Bxxx-tiny.out

${BINDIR}/calter: obj.d/calter.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

calter-test: ${BINDIR}/calter calter-tiny.in calter-tiny.out
	timeout 8 ${BINDIR}/calter -naive calter-tiny.in calter-tiny.xnout
	diff calter-tiny.xnout calter-tiny.out
	timeout 8 ${BINDIR}/calter calter-tiny.in calter-tiny.xout
	diff calter-tiny.xout calter-tiny.out

${BINDIR}/dsumxor: obj.d/dsumxor.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

dsumxor-test: ${BINDIR}/dsumxor dsumxor-tiny.in dsumxor-tiny.out
	timeout 8 ${BINDIR}/dsumxor -naive dsumxor-tiny.in dsumxor-tiny.xnout
	diff dsumxor-tiny.xnout dsumxor-tiny.out
	timeout 8 ${BINDIR}/dsumxor dsumxor-tiny.in dsumxor-tiny.xout
	diff dsumxor-tiny.xout dsumxor-tiny.out

${BINDIR}/Exxx: obj.d/Exxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Exxx-test: ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.out
	timeout 8 ${BINDIR}/Exxx -naive Exxx-tiny.in Exxx-tiny.xnout
	diff Exxx-tiny.xnout Exxx-tiny.out
	timeout 8 ${BINDIR}/Exxx Exxx-tiny.in Exxx-tiny.xout
	diff Exxx-tiny.xout Exxx-tiny.out

${BINDIR}/Fxxx: obj.d/Fxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Fxxx-test: ${BINDIR}/Fxxx Fxxx-tiny.in Fxxx-tiny.out
	timeout 8 ${BINDIR}/Fxxx -naive Fxxx-tiny.in Fxxx-tiny.xnout
	diff Fxxx-tiny.xnout Fxxx-tiny.out
	timeout 8 ${BINDIR}/Fxxx Fxxx-tiny.in Fxxx-tiny.xout
	diff Fxxx-tiny.xout Fxxx-tiny.out

${BINDIR}/Gxxx: obj.d/Gxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Gxxx-test: ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.out
	timeout 8 ${BINDIR}/Gxxx -naive Gxxx-tiny.in Gxxx-tiny.xnout
	diff Gxxx-tiny.xnout Gxxx-tiny.out
	timeout 8 ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.xout
	diff Gxxx-tiny.xout Gxxx-tiny.out

${BINDIR}/Hxxx: obj.d/Hxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Hxxx-test: ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.out
	timeout 8 ${BINDIR}/Hxxx -naive Hxxx-tiny.in Hxxx-tiny.xnout
	diff Hxxx-tiny.xnout Hxxx-tiny.out
	timeout 8 ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.xout
	diff Hxxx-tiny.xout Hxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
