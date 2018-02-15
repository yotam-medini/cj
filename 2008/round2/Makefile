#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a
LD_LIBGMP=-Wl,-L${LIBDIR} -Wl,-R${LIBDIR} -lgmpxx -lgmp

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

${BINDIR}/cheating: obj.d/cheating.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cheating-test: ${BINDIR}/cheating cheating-tiny.in cheating-tiny.out
	${BINDIR}/cheating cheating-tiny.in cheating-tiny.xout
	diff cheating-tiny.xout cheating-tiny.out

${BINDIR}/triangles: obj.d/triangles.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

triangles-test: ${BINDIR}/triangles triangles-tiny.in triangles-tiny.out
	${BINDIR}/triangles -small triangles-tiny.in triangles-tiny.xout
	diff triangles-tiny.xout triangles-tiny.out

${BINDIR}/starwars: obj.d/starwars.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LD_LIBGMP}
	ldd $@

starwars-test: ${BINDIR}/starwars starwars-tiny.in starwars-tiny.out
	${BINDIR}/starwars -naive starwars-tiny.in starwars-tiny.xnout
	diff starwars-tiny.xnout starwars-tiny.out
	${BINDIR}/starwars starwars-tiny.in starwars-tiny.xout
	diff starwars-tiny.xout starwars-tiny.out

${BINDIR}/permrle: obj.d/permrle.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

permrle-test: ${BINDIR}/permrle permrle-tiny.in permrle-tiny.out
	${BINDIR}/permrle -naive permrle-tiny.in permrle-tiny.xnout
	diff permrle-tiny.xnout permrle-tiny.out
	${BINDIR}/permrle permrle-tiny.in permrle-tiny.xout
	diff permrle-tiny.xout permrle-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
