#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2019/April/28
# https://codingcompetitions.withgoogle.com/codejam/round/0000000000051706

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

${BINDIR}/cart: obj.d/cart.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

cart-test: ${BINDIR}/cart cart-tiny.in cart-tiny.out
	timeout 8 ${BINDIR}/cart -naive cart-tiny.in cart-tiny.xnout
	diff cart-tiny.xnout cart-tiny.out

dum:
	timeout 8 ${BINDIR}/cart cart-tiny.in cart-tiny.xout
	diff cart-tiny.xout cart-tiny.out

${BINDIR}/draupnir: obj.d/draupnir.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

draupnir-test: ${BINDIR}/draupnir my-interactive_runner.py my-testing_tool.py 
	timeout 8 ./my-interactive_runner.py ./my-testing_tool.py 0 -- ${BINDIR}/draupnir 
	timeout 8 ./my-interactive_runner.py ./my-testing_tool.py 1 -- ${BINDIR}/draupnir 

${BINDIR}/fight: obj.d/fight.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

fight-test: ${BINDIR}/fight fight-tiny.in fight-tiny.out
	timeout 8 ${BINDIR}/fight -naive fight-tiny.in fight-tiny.xnout
	diff fight-tiny.xnout fight-tiny.out
	timeout 8 ${BINDIR}/fight fight-tiny.in fight-tiny.xout
	diff fight-tiny.xout fight-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
