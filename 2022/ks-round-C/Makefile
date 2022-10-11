# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2022/October/07
# codingcompetitions.withgoogle.com/kickstart/round/00000000008cb4d1a

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

${BINDIR}/%: obj.d/%.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

define run_test_123
.PHONY: ${3}

${3}: ${BINDIR}/${1} ${2}.in ${2}.out
	timeout 2 ${BINDIR}/${1} -naive ${2}.in ${2}.xnout
	diff ${2}.xnout ${2}.out
	timeout 2 ${BINDIR}/${1} ${2}.in ${2}.xout
	diff ${2}.xout ${2}.out
endef

define run_test
$(eval $(call run_test_123,${1},${1}-tiny,${1}-test))
endef

define run_test2
.PHONY: ${1}-test

${1}-test: ${1}-test1 ${1}-test2
$(call run_test_123,${1},${1}-tiny,${1}-test1)
$(call run_test_123,${1},${1}-tiny2,${1}-test2)
endef

# $(eval $(call run_test,xxx))
# $(eval $(call run_test2,xxx))

newpw-test: ${BINDIR}/newpw \
	newpw-tiny.in newpw-tiny.out newpw-tiny2.in newpw-tiny2.out
	timeout 2 ${BINDIR}/newpw newpw-tiny.in newpw-tiny.xout
	diff <(wc < newpw-tiny.xout) <(wc < newpw-tiny.out)
	timeout 2 ${BINDIR}/newpw newpw-tiny2.in newpw-tiny2.xout
	diff <(wc < newpw-tiny2.xout) <(wc < newpw-tiny2.out)

rangepart-test: ${BINDIR}/rangepart rangepart-tiny.in rangepart-tiny.out
	timeout 2 ${BINDIR}/rangepart -naive rangepart-tiny.in rangepart-tiny.xnout
	diff rangepart-tiny.xnout rangepart-tiny.out
	timeout 2 ${BINDIR}/rangepart rangepart-tiny.in rangepart-tiny.xout
	diff <(grep POSSIBLE < rangepart-tiny.xout) <(grep POSSIBLE < rangepart-tiny.out)

# $(eval $(call run_test,rangepart))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
