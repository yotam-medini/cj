#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

LIBDIR=/usr/lib/i386-linux-gnu
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

${BINDIR}/snapper: obj.d/snapper.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

snapper-test: ${BINDIR}/snapper snapper-tiny.in snapper-tiny.out
	${BINDIR}/snapper snapper-tiny.in snapper-tiny.xout
	diff snapper-tiny.xout snapper-tiny.out

${BINDIR}/fairwarn: obj.d/fairwarn.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

fairwarn-test: ${BINDIR}/fairwarn fairwarn-tiny.in fairwarn-tiny.out
	${BINDIR}/fairwarn fairwarn-tiny.in fairwarn-tiny.xout
	diff fairwarn-tiny.xout fairwarn-tiny.out

${BINDIR}/theme: obj.d/theme.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

theme-test: ${BINDIR}/theme theme-tiny.in theme-tiny.out
	${BINDIR}/theme theme-tiny.in theme-tiny.xnout 0 -naive
	diff theme-tiny.xnout theme-tiny.out
	${BINDIR}/theme theme-tiny.in theme-tiny.xout
	diff theme-tiny.xout theme-tiny.out

clean:
	rm -f ${OBJS}

-include ${DEPS}


