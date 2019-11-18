#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/May/05
# codingcompetitions.withgoogle.com/codejam/round/0000000000007765

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/newword: obj.d/newword.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

newword-test: ${BINDIR}/newword newword-tiny.in newword-tiny.out
	timeout 8 ${BINDIR}/newword newword-tiny.in newword-tiny.xout
	diff newword-tiny.xout newword-tiny.out

${BINDIR}/lolli: obj.d/lolli.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lolli-test: ${BINDIR}/lolli testing_tool.py
	python ./testing_tool.py ${BINDIR}/lolli -debug 0x1

${BINDIR}/antstack: obj.d/antstack.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

antstack-test: ${BINDIR}/antstack antstack-tiny.in antstack-tiny.out
	timeout 8 ${BINDIR}/antstack antstack-tiny.in antstack-tiny.xout
	diff antstack-tiny.xout antstack-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
