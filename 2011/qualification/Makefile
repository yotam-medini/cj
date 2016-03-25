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

${BINDIR}/bot: obj.d/bot.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bot-test: ${BINDIR}/bot bot-tiny.in bot-tiny.out
	${BINDIR}/bot bot-tiny.in bot-tiny.xout
	diff bot-tiny.xout bot-tiny.out

${BINDIR}/magicka: obj.d/magicka.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

magicka-test: ${BINDIR}/magicka magicka-tiny.in magicka-tiny.out
	${BINDIR}/magicka magicka-tiny.in magicka-tiny.xout
	diff magicka-tiny.xout magicka-tiny.out

${BINDIR}/candy: obj.d/candy.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

candy-test: ${BINDIR}/candy candy-tiny.in candy-tiny.out
	${BINDIR}/candy candy-tiny.in candy-tiny.xout
	diff candy-tiny.xout candy-tiny.out

xx-test: ${BINDIR}/xx xx-tiny.in xx-tiny.out
	${BINDIR}/xx xx-tiny.in xx-tiny.xnout 0 -naive
	diff xx-tiny.xnout xx-tiny.out
	${BINDIR}/xx xx-tiny.in xx-tiny.xout
	diff xx-tiny.xout xx-tiny.out

clean:
	rm -f ${OBJS}

-include ${DEPS}


