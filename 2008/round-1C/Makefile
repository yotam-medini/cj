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

${BINDIR}/textmsg: obj.d/textmsg.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

textmsg-test: ${BINDIR}/textmsg textmsg-tiny.in textmsg-tiny.out
	${BINDIR}/textmsg textmsg-tiny.in textmsg-tiny.xout
	diff textmsg-tiny.xout textmsg-tiny.out

${BINDIR}/uglynums: obj.d/uglynums.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

uglynums-test: ${BINDIR}/uglynums uglynums-tiny.in uglynums-tiny.out
	${BINDIR}/uglynums uglynums-tiny.in uglynums-tiny.xout
	diff uglynums-tiny.xout uglynums-tiny.out

${BINDIR}/incspeed: obj.d/incspeed.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

incspeed-test: ${BINDIR}/incspeed incspeed-tiny.in incspeed-tiny.out
	${BINDIR}/incspeed incspeed-tiny.in incspeed-tiny.xout
	diff incspeed-tiny.xout incspeed-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
