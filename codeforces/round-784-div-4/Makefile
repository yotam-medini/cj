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
	timeout 2 ${BINDIR}/adiv -naive adiv-tiny.in adiv-tiny.xnout
	diff adiv-tiny.xnout adiv-tiny.out
	timeout 2 ${BINDIR}/adiv adiv-tiny.in adiv-tiny.xout
	diff adiv-tiny.xout adiv-tiny.out

${BINDIR}/btriple: obj.d/btriple.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

btriple-test: ${BINDIR}/btriple btriple-tiny.in btriple-tiny.out
	timeout 8 ${BINDIR}/btriple -naive btriple-tiny.in btriple-tiny.xnout
	diff btriple-tiny.xnout btriple-tiny.out
	timeout 8 ${BINDIR}/btriple btriple-tiny.in btriple-tiny.xout
	diff btriple-tiny.xout btriple-tiny.out

${BINDIR}/coe: obj.d/coe.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

coe-test: ${BINDIR}/coe coe-tiny.in coe-tiny.out
	timeout 8 ${BINDIR}/coe -naive coe-tiny.in coe-tiny.xnout
	diff coe-tiny.xnout coe-tiny.out
	timeout 8 ${BINDIR}/coe coe-tiny.in coe-tiny.xout
	diff coe-tiny.xout coe-tiny.out

${BINDIR}/dcolor: obj.d/dcolor.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

dcolor-test: ${BINDIR}/dcolor dcolor-tiny.in dcolor-tiny.out
	timeout 2 ${BINDIR}/dcolor -naive dcolor-tiny.in dcolor-tiny.xnout
	diff dcolor-tiny.xnout dcolor-tiny.out
	timeout 2 ${BINDIR}/dcolor dcolor-tiny.in dcolor-tiny.xout
	diff dcolor-tiny.xout dcolor-tiny.out

${BINDIR}/e2letter: obj.d/e2letter.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

e2letter-test: ${BINDIR}/e2letter e2letter-tiny.in e2letter-tiny.out
	timeout 2 ${BINDIR}/e2letter -naive e2letter-tiny.in e2letter-tiny.xnout
	diff e2letter-tiny.xnout e2letter-tiny.out
	timeout 2 ${BINDIR}/e2letter e2letter-tiny.in e2letter-tiny.xout
	diff e2letter-tiny.xout e2letter-tiny.out

${BINDIR}/feat: obj.d/feat.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

feat-test: ${BINDIR}/feat feat-tiny.in feat-tiny.out
	timeout 2 ${BINDIR}/feat -naive feat-tiny.in feat-tiny.xnout
	diff feat-tiny.xnout feat-tiny.out
	timeout 2 ${BINDIR}/feat feat-tiny.in feat-tiny.xout
	diff feat-tiny.xout feat-tiny.out

${BINDIR}/Gxxx: obj.d/Gxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Gxxx-test: ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.out
	timeout 2 ${BINDIR}/Gxxx -naive Gxxx-tiny.in Gxxx-tiny.xnout
	diff Gxxx-tiny.xnout Gxxx-tiny.out
	timeout 2 ${BINDIR}/Gxxx Gxxx-tiny.in Gxxx-tiny.xout
	diff Gxxx-tiny.xout Gxxx-tiny.out

${BINDIR}/Hxxx: obj.d/Hxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

Hxxx-test: ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.out
	timeout 2 ${BINDIR}/Hxxx -naive Hxxx-tiny.in Hxxx-tiny.xnout
	diff Hxxx-tiny.xnout Hxxx-tiny.out
	timeout 2 ${BINDIR}/Hxxx Hxxx-tiny.in Hxxx-tiny.xout
	diff Hxxx-tiny.xout Hxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
