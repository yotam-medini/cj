#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# https://codingcompetitions.withgoogle.com/codejam/round/0000000000051635

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

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

${BINDIR}/pylon: obj.d/pylon.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

pylon-test: ${BINDIR}/pylon test-pylon.py
	timeout 240 ./test-pylon.py

${BINDIR}/golf: obj.d/golf.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

golf-test: ${BINDIR}/golf interactive_runner.py testing_tool.py
	python interactive_runner.py python testing_tool.py  0 -- ./bin/golf

${BINDIR}/rhyme: obj.d/rhyme.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rhyme-test: ${BINDIR}/rhyme rhyme-tiny.in rhyme-tiny.out
	timeout 8 ${BINDIR}/rhyme rhyme-tiny.in rhyme-tiny.xout
	diff rhyme-tiny.xout rhyme-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
