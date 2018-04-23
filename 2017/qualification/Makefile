#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/ospanflip: obj.d/ospanflip.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

ospanflip-test-naive: ${BINDIR}/ospanflip ospanflip-tiny.in ospanflip-tiny.out
	${BINDIR}/ospanflip -naive ospanflip-tiny.in ospanflip-tiny.xnout
	diff ospanflip-tiny.xnout ospanflip-tiny.out

ospanflip-test: ${BINDIR}/ospanflip ospanflip-tiny.in ospanflip-tiny.out
	${BINDIR}/ospanflip ospanflip-tiny.in ospanflip-tiny.xout
	diff ospanflip-tiny.xout ospanflip-tiny.out

${BINDIR}/tidynum: obj.d/tidynum.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

tidynum-test-naive: ${BINDIR}/tidynum tidynum-tiny.in tidynum-tiny.out
	${BINDIR}/tidynum -naive tidynum-tiny.in tidynum-tiny.xnout
	diff tidynum-tiny.xnout tidynum-tiny.out

tidynum-test: ${BINDIR}/tidynum tidynum-example.in tidynum-example.out
	${BINDIR}/tidynum tidynum-example.in tidynum-example.xout
	diff tidynum-example.xout tidynum-example.out

${BINDIR}/bathstall: obj.d/bathstall.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bathstall-test-naive: ${BINDIR}/bathstall bathstall-tiny.in bathstall-tiny.out
	${BINDIR}/bathstall -naive bathstall-tiny.in bathstall-tiny.xout
	diff -u bathstall-tiny.xout bathstall-tiny.out

bathstall-test: ${BINDIR}/bathstall bathstall-tiny.in bathstall-tiny.out
	${BINDIR}/bathstall bathstall-tiny.in bathstall-tiny.xout
	diff -u bathstall-tiny.xout bathstall-tiny.out

${BINDIR}/fashion: obj.d/fashion.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

fashion-test: ${BINDIR}/fashion fashion-tiny.in fashion-tiny.out
	${BINDIR}/fashion fashion-tiny.in fashion-tiny.xout
	cat fashion-tiny.xout

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
