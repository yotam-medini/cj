# codingcompetitions.withgoogle.com/kickstart/round/0000000000436140/
# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

SHELL=/bin/bash
DBGFLAGS = -g
OPTFLAGS = ${DBGFLAGS}
CFLAGS = ${OPTFLAGS} -Wall -Wshadow -std=c++17 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/kgood: obj.d/kgood.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

kgood-test: ${BINDIR}/kgood kgood-tiny.in kgood-tiny.out
	timeout 2 ${BINDIR}/kgood -naive kgood-tiny.in kgood-tiny.xnout
	diff kgood-tiny.xnout kgood-tiny.out
	timeout 2 ${BINDIR}/kgood kgood-tiny.in kgood-tiny.xout
	diff kgood-tiny.xout kgood-tiny.out

${BINDIR}/lshape: obj.d/lshape.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

lshape-test: ${BINDIR}/lshape lshape-tiny.in lshape-tiny.out
	timeout 2 ${BINDIR}/lshape -naive lshape-tiny.in lshape-tiny.xnout
	diff lshape-tiny.xnout lshape-tiny.out
	timeout 2 ${BINDIR}/lshape lshape-tiny.in lshape-tiny.xout
	diff lshape-tiny.xout lshape-tiny.out

${BINDIR}/rabbit: obj.d/rabbit.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rabbit-test: ${BINDIR}/rabbit rabbit-tiny.in rabbit-tiny.out
	timeout 2 ${BINDIR}/rabbit -naive rabbit-tiny.in rabbit-tiny.xnout
	diff rabbit-tiny.xnout rabbit-tiny.out
	timeout 2 ${BINDIR}/rabbit rabbit-tiny.in rabbit-tiny.xout
	diff rabbit-tiny.xout rabbit-tiny.out

${BINDIR}/checksum: obj.d/checksum.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

checksum-test: ${BINDIR}/checksum checksum-tiny.in checksum-tiny.out
	timeout 2 ${BINDIR}/checksum -naive checksum-tiny.in checksum-tiny.xnout
	diff checksum-tiny.xnout checksum-tiny.out
	timeout 2 ${BINDIR}/checksum checksum-tiny.in checksum-tiny.xout
	diff checksum-tiny.xout checksum-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
