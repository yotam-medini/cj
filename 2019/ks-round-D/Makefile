#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# https://codingcompetitions.withgoogle.com/kickstart/round/0000000000051061/

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

${BINDIR}/xwhat: obj.d/xwhat.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

xwhat-test: ${BINDIR}/xwhat xwhat-tiny.in xwhat-tiny.out
	timeout 8 ${BINDIR}/xwhat -naive xwhat-tiny.in xwhat-tiny.xnout
	diff xwhat-tiny.xnout xwhat-tiny.out
	timeout 8 ${BINDIR}/xwhat xwhat-tiny.in xwhat-tiny.xout
	diff xwhat-tiny.xout xwhat-tiny.out

${BINDIR}/lguests: obj.d/lguests.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lguests-test: ${BINDIR}/lguests lguests-tiny.in lguests-tiny.out
	timeout 8 ${BINDIR}/lguests -naive lguests-tiny.in lguests-tiny.xnout
	diff lguests-tiny.xnout lguests-tiny.out
	timeout 8 ${BINDIR}/lguests lguests-tiny.in lguests-tiny.xout
	diff lguests-tiny.xout lguests-tiny.out

${BINDIR}/fstall: obj.d/fstall.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

fstall-test: ${BINDIR}/fstall fstall-tiny.in fstall-tiny.out
	timeout 8 ${BINDIR}/fstall -naive fstall-tiny.in fstall-tiny.xnout
	diff fstall-tiny.xnout fstall-tiny.out
	timeout 8 ${BINDIR}/fstall -naive1 fstall-tiny.in fstall-tiny.xn1out
	diff fstall-tiny.xn1out fstall-tiny.out
	timeout 8 ${BINDIR}/fstall -naive2 fstall-tiny.in fstall-tiny.xn2out
	diff fstall-tiny.xn2out fstall-tiny.out
	timeout 8 ${BINDIR}/fstall fstall-tiny.in fstall-tiny.xout
	diff fstall-tiny.xout fstall-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
