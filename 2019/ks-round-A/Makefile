#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/March/23

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

${BINDIR}/train: obj.d/train.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

train-test: ${BINDIR}/train train-tiny.in train-tiny.out
	timeout 8 ${BINDIR}/train -naive train-tiny.in train-tiny.xnout
	diff train-tiny.xnout train-tiny.out
	timeout 8 ${BINDIR}/train train-tiny.in train-tiny.xout
	diff train-tiny.xout train-tiny.out

${BINDIR}/parcel: obj.d/parcel.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

parcel-test: ${BINDIR}/parcel parcel-tiny.in parcel-tiny.out
	timeout 8 ${BINDIR}/parcel parcel-tiny.in parcel-tiny.xout
	diff parcel-tiny.xout parcel-tiny.out

${BINDIR}/contention: obj.d/contention.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

contention-test: ${BINDIR}/contention contention-tiny.in contention-tiny.out
	timeout 8 ${BINDIR}/contention contention-tiny.in contention-tiny.xout
	diff contention-tiny.xout contention-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
