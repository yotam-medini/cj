#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ffc8

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

${BINDIR}/bike: obj.d/bike.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bike-test: ${BINDIR}/bike bike-tiny.in bike-tiny.out
	timeout 8 ${BINDIR}/bike -naive bike-tiny.in bike-tiny.xnout
	diff bike-tiny.xnout bike-tiny.out
	timeout 8 ${BINDIR}/bike bike-tiny.in bike-tiny.xout
	diff bike-tiny.xout bike-tiny.out

${BINDIR}/bus: obj.d/bus.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

bus-test: ${BINDIR}/bus bus-tiny.in bus-tiny.out
	timeout 8 ${BINDIR}/bus -naive bus-tiny.in bus-tiny.xnout
	diff bus-tiny.xnout bus-tiny.out
	timeout 8 ${BINDIR}/bus bus-tiny.in bus-tiny.xout
	diff bus-tiny.xout bus-tiny.out

${BINDIR}/robot: obj.d/robot.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

robot-test: ${BINDIR}/robot robot-tiny.in robot-tiny.out
	timeout 8 ${BINDIR}/robot -naive robot-tiny.in robot-tiny.xnout
	diff robot-tiny.xnout robot-tiny.out
	timeout 8 ${BINDIR}/robot robot-tiny.in robot-tiny.xout
	diff robot-tiny.xout robot-tiny.out

${BINDIR}/wander: obj.d/wander.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

wander-test: ${BINDIR}/wander wander-tiny.in wander-tiny.out
	timeout 8 ${BINDIR}/wander -naive wander-tiny.in wander-tiny.xnout
	diff wander-tiny.xnout wander-tiny.out
	timeout 8 ${BINDIR}/wander wander-tiny.in wander-tiny.xout
	diff wander-tiny.xout wander-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
