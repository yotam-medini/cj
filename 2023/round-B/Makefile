# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/0000000000c9607c

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

$(eval $(call run_test,collecting))
$(eval $(call run_test,intruder))
$(eval $(call run_test,spacious))
$(eval $(call run_test,railroad))
$(eval $(call run_test,railmanage))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
