#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:  2020/March/21

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

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

${BINDIR}/alloc: obj.d/alloc.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

alloc-test: ${BINDIR}/alloc alloc-tiny.in alloc-tiny.out
	timeout 8 ${BINDIR}/alloc -naive alloc-tiny.in alloc-tiny.xnout
	diff alloc-tiny.xnout alloc-tiny.out
	timeout 8 ${BINDIR}/alloc alloc-tiny.in alloc-tiny.xout
	diff alloc-tiny.xout alloc-tiny.out

${BINDIR}/plates: obj.d/plates.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

plates-test: ${BINDIR}/plates plates-tiny.in plates-tiny.out
	timeout 8 ${BINDIR}/plates -naive plates-tiny.in plates-tiny.xnout
	diff plates-tiny.xnout plates-tiny.out
	timeout 8 ${BINDIR}/plates plates-tiny.in plates-tiny.xout
	diff plates-tiny.xout plates-tiny.out

${BINDIR}/workout: obj.d/workout.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

workout-test: ${BINDIR}/workout workout-tiny.in workout-tiny.out
	timeout 8 ${BINDIR}/workout -naive workout-tiny.in workout-tiny.xnout
	diff workout-tiny.xnout workout-tiny.out
	timeout 8 ${BINDIR}/workout workout-tiny.in workout-tiny.xout
	diff workout-tiny.xout workout-tiny.out
	timeout 8 ${BINDIR}/workout workout-tiny2.in workout-tiny2.xout
	diff workout-tiny2.xout workout-tiny2.out

${BINDIR}/bundling: obj.d/bundling.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bundling-test: ${BINDIR}/bundling bundling-tiny.in bundling-tiny.out
	timeout 8 ${BINDIR}/bundling -naive bundling-tiny.in bundling-tiny.xnout
	diff bundling-tiny.xnout bundling-tiny.out
	timeout 8 ${BINDIR}/bundling bundling-tiny.in bundling-tiny.xout
	diff bundling-tiny.xout bundling-tiny.out
	timeout 8 ${BINDIR}/bundling -naive bundling-tiny2.in bundling-tiny2.xnout
	diff bundling-tiny2.xnout bundling-tiny2.out
	timeout 8 ${BINDIR}/bundling bundling-tiny2.in bundling-tiny2.xout
	diff bundling-tiny2.xout bundling-tiny2.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
