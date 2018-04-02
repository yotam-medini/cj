# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/March/30

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

${BINDIR}/numguess: obj.d/numguess.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

numguess-test: ${BINDIR}/numguess testing_tool.py
	python ./testing_tool.py ${BINDIR}/numguess

${BINDIR}/senate: obj.d/senate.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

senate-test: ${BINDIR}/senate senate-tiny.in senate-tiny.out
	${BINDIR}/senate senate-tiny.in senate-tiny.xout
	diff senate-tiny.xout senate-tiny.out

${BINDIR}/cruise: obj.d/cruise.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cruise-test: ${BINDIR}/cruise cruise-tiny.in cruise-tiny.out
	${BINDIR}/cruise cruise-tiny.in cruise-tiny.xout
	diff cruise-tiny.xout cruise-tiny.out

${BINDIR}/bathstalls: obj.d/bathstalls.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

bathstalls-test: ${BINDIR}/bathstalls bathstalls-tiny.in bathstalls-tiny.out
	${BINDIR}/bathstalls -naive bathstalls-tiny.in bathstalls-tiny.xnout
	diff bathstalls-tiny.xnout bathstalls-tiny.out
	${BINDIR}/bathstalls bathstalls-tiny.in bathstalls-tiny.xout
	diff bathstalls-tiny.xout bathstalls-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
