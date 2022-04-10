#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 
# codingcompetitions.withgoogle.com/codejam/round/0000000000435baf
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

${BINDIR}/clock: obj.d/clock.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

clock-test: ${BINDIR}/clock clock-tiny.in clock-tiny.out \
	clock-tiny2.in clock-tiny2.out \
	clock-tiny3.in clock-tiny3.out
	timeout 2 ${BINDIR}/clock clock-tiny.in clock-tiny.xout
	diff clock-tiny.xout clock-tiny.out || true
	timeout 2 ${BINDIR}/clock clock-tiny2.in clock-tiny2.xout
	diff clock-tiny2.xout clock-tiny2.out
	timeout 2 ${BINDIR}/clock clock-tiny3.in clock-tiny3.xout
	diff clock-tiny3.xout clock-tiny3.out

${BINDIR}/subx: obj.d/subx.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

subx-test: ${BINDIR}/subx subx-tiny.in subx-tiny.out
	timeout 2 ${BINDIR}/subx -naive subx-tiny.in subx-tiny.xnout
	diff subx-tiny.xnout subx-tiny.out
	timeout 2 ${BINDIR}/subx subx-tiny.in subx-tiny.xout
	diff subx-tiny.xout subx-tiny.out
	timeout 2 ${BINDIR}/subx -naive subx-tiny2.in subx-tiny2.xnout
	diff subx-tiny2.xnout subx-tiny2.out
	timeout 2 ${BINDIR}/subx subx-tiny2.in subx-tiny2.xout
	diff subx-tiny2.xout subx-tiny2.out

${BINDIR}/digit: obj.d/digit.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

DIGIT_DEBUG=0x0
DIGIT_TIMEOUT=10
DIGIT_TEST_REQ = ${BINDIR}/digit interactive_runner.py local_testing_tool.py
digit-test: digit-test-0 digit-test-1

define DIGIT_TEST_N =
$(1): ${DIGIT_TEST_REQ}
	@echo; echo $(1):
	timeout ${DIGIT_TIMEOUT} python interactive_runner.py \
	  python local_testing_tool.py $(2) -- \
	  ./${BINDIR}/digit -debug ${DIGIT_DEBUG}
endef

$(eval $(call DIGIT_TEST_N, digit-test-0, 0))

$(eval $(call DIGIT_TEST_N, digit-test-1, 1))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
