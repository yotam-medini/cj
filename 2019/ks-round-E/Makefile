#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/August/25
# https://codingcompetitions.withgoogle.com/kickstart/round/0000000000050edb/

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

${BINDIR}/cherries: obj.d/cherries.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cherries-test: ${BINDIR}/cherries cherries-tiny.in cherries-tiny.out
	timeout 8 ${BINDIR}/cherries -naive cherries-tiny.in cherries-tiny.xnout
	diff cherries-tiny.xnout cherries-tiny.out
	timeout 8 ${BINDIR}/cherries cherries-tiny.in cherries-tiny.xout
	diff cherries-tiny.xout cherries-tiny.out

${BINDIR}/codeeat: obj.d/codeeat.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

codeeat-test: ${BINDIR}/codeeat codeeat-tiny.in codeeat-tiny.out
	timeout 8 ${BINDIR}/codeeat -naive codeeat-tiny.in codeeat-tiny.xnout
	diff codeeat-tiny.xnout codeeat-tiny.out
	timeout 8 ${BINDIR}/codeeat codeeat-tiny.in codeeat-tiny.xout
	diff codeeat-tiny.xout codeeat-tiny.out

${BINDIR}/streetchk: obj.d/streetchk.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

streetchk-test: ${BINDIR}/streetchk streetchk-tiny.in streetchk-tiny.out
	timeout 8 ${BINDIR}/streetchk -naive streetchk-tiny.in streetchk-tiny.xnout
	diff streetchk-tiny.xnout streetchk-tiny.out
	timeout 8 ${BINDIR}/streetchk streetchk-tiny.in streetchk-tiny.xout
	diff streetchk-tiny.xout streetchk-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
