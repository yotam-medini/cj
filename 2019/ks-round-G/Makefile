#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/0000000000050e02

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

${BINDIR}/book: obj.d/book.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

book-test: ${BINDIR}/book book-tiny.in book-tiny.out
	timeout 8 ${BINDIR}/book -naive book-tiny.in book-tiny.xnout
	diff book-tiny.xnout book-tiny.out
	timeout 8 ${BINDIR}/book book-tiny.in book-tiny.xout
	diff book-tiny.xout book-tiny.out

${BINDIR}/equation: obj.d/equation.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

equation-test: ${BINDIR}/equation equation-tiny.in equation-tiny.out
	timeout 8 ${BINDIR}/equation -naive equation-tiny.in equation-tiny.xnout
	diff equation-tiny.xnout equation-tiny.out
	timeout 8 ${BINDIR}/equation equation-tiny.in equation-tiny.xout
	diff equation-tiny.xout equation-tiny.out

${BINDIR}/shifts: obj.d/shifts.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

shifts-test: ${BINDIR}/shifts shifts-tiny.in shifts-tiny.out
	timeout 8 ${BINDIR}/shifts -naive shifts-tiny.in shifts-tiny.xnout
	diff shifts-tiny.xnout shifts-tiny.out
	timeout 8 ${BINDIR}/shifts shifts-tiny.in shifts-tiny.xout
	diff shifts-tiny.xout shifts-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
