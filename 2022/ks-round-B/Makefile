# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/kickstart/round/00000000008caa74

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

define run_test_123_possible
.PHONY: ${3}

${3}: ${BINDIR}/${1} ${2}.in ${2}.out
	timeout 2 ${BINDIR}/${1} -naive ${2}.in ${2}.xnout
	diff <(grep POSSIBLE < ${2}.xnout) <(grep POSSIBLE < ${2}.out)
	timeout 2 ${BINDIR}/${1} ${2}.in ${2}.xout
	diff <(grep POSSIBLE < ${2}.xout) <(grep POSSIBLE < ${2}.out)
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

define run_test_possible
$(eval $(call run_test_123_possible,${1},${1}-tiny,${1}-test))
endef

define run_test2_possible
.PHONY: ${1}-test

${1}-test: ${1}-test1 ${1}-test2
$(call run_test_123_possible,${1},${1}-tiny,${1}-test1)
$(call run_test_123_possible,${1},${1}-tiny2,${1}-test2)
endef

$(eval $(call run_test,infiarea))
$(eval $(call run_test,palifactor))
$(eval $(call run_test2,padlock))
$(eval $(call run_test2_possible,hamilton))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
