#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/000000000019ff49

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

${BINDIR}/retype: obj.d/retype.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

retype-test: ${BINDIR}/retype retype-tiny.in retype-tiny.out
	timeout 3 ${BINDIR}/retype -naive retype-tiny.in retype-tiny.xnout
	diff retype-tiny.xnout retype-tiny.out
	timeout 3 ${BINDIR}/retype retype-tiny.in retype-tiny.xout
	diff retype-tiny.xout retype-tiny.out

${BINDIR}/boring: obj.d/boring.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

boring-test: ${BINDIR}/boring boring-tiny.in boring-tiny.out
	timeout 3 ${BINDIR}/boring -naive boring-tiny.in boring-tiny.xnout
	diff boring-tiny.xnout boring-tiny.out
	timeout 3 ${BINDIR}/boring boring-tiny.in boring-tiny.xout
	diff boring-tiny.xout boring-tiny.out

${BINDIR}/rugby: obj.d/rugby.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

rugby-test: ${BINDIR}/rugby rugby-tiny.in rugby-tiny.out
	timeout 3 ${BINDIR}/rugby -naive rugby-tiny.in rugby-tiny.xnout
	diff rugby-tiny.xnout rugby-tiny.out
	timeout 3 ${BINDIR}/rugby rugby-tiny.in rugby-tiny.xout
	diff rugby-tiny.xout rugby-tiny.out

${BINDIR}/friends: obj.d/friends.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

friends-test: ${BINDIR}/friends friends-tiny.in friends-tiny.out
	timeout 3 ${BINDIR}/friends -naive friends-tiny.in friends-tiny.xnout
	diff friends-tiny.xnout friends-tiny.out
	timeout 3 ${BINDIR}/friends friends-tiny.in friends-tiny.xout
	diff friends-tiny.xout friends-tiny.out

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
