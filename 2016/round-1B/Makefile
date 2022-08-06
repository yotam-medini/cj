#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2016/March/20
# codingcompetitions.withgoogle.com/codejam/round/0000000000201b6c

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

${BINDIR}/digits: obj.d/digits.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

digits-test: ${BINDIR}/digits digits-tiny.in digits-tiny.out
	${BINDIR}/digits digits-tiny.in digits-tiny.xout
	diff digits-tiny.xout digits-tiny.out

${BINDIR}/closematch: obj.d/closematch.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

closematch-test: ${BINDIR}/closematch closematch-tiny.in closematch-tiny.out
	${BINDIR}/closematch closematch-tiny.in closematch-tiny.xout
	diff closematch-tiny.xout closematch-tiny.out

${BINDIR}/techno: obj.d/techno.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

techno-test: ${BINDIR}/techno techno-tiny.in techno-tiny.out
	${BINDIR}/techno techno-tiny.in techno-tiny.xout
	diff techno-tiny.xout techno-tiny.out

${BINDIR}/Dxxx: obj.d/Dxxx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^ ${LIBGMP}

Dxxx-test: ${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.out
	${BINDIR}/Dxxx Dxxx-tiny.in Dxxx-tiny.xout
	diff Dxxx-tiny.xout Dxxx-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
