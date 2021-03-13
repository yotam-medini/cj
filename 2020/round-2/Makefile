#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/000000000019ffb9

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

${BINDIR}/ihp: obj.d/ihp.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ihp-test: ${BINDIR}/ihp ihp-tiny.in ihp-tiny.out
	timeout 2 ${BINDIR}/ihp -naive ihp-tiny.in ihp-tiny.xnout
	diff ihp-tiny.xnout ihp-tiny.out
	timeout 2 ${BINDIR}/ihp ihp-tiny.in ihp-tiny.xout
	diff ihp-tiny.xout ihp-tiny.out

${BINDIR}/security: obj.d/security.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

VERIFY = -debug 0x100
security-test: ${BINDIR}/security security-tiny.in
	timeout 2 ${BINDIR}/security -naive ${VERIFY} \
	  security-tiny.in security-tiny.xnout
	timeout 2 ${BINDIR}/security ${VERIFY} \
	  security-tiny.in security-tiny.xout
	timeout 2 ${BINDIR}/security ${VERIFY} \
	  -naive security-tiny2.in security-tiny2.xnout
	timeout 2 ${BINDIR}/security ${VERIFY} \
	  security-tiny2.in security-tiny2.xout

${BINDIR}/wormhole: obj.d/wormhole.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

wormhole-test: ${BINDIR}/wormhole wormhole-tiny.in wormhole-tiny.out
	timeout 2 ${BINDIR}/wormhole -naive wormhole-tiny.in wormhole-tiny.xnout
	diff wormhole-tiny.xnout wormhole-tiny.out
	timeout 2 ${BINDIR}/wormhole wormhole-tiny.in wormhole-tiny.xout
	diff wormhole-tiny.xout wormhole-tiny.out

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
