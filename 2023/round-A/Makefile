# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/0000000000c95b94
#

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

$(eval $(call run_test,colliding))
$(eval $(call run_test,illum))
$(eval $(call run_test,rainbow))
$(eval $(call run_test,ascii))
$(eval $(call run_test,untie))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
