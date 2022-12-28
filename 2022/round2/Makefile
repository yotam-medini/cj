# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# https://codingcompetitions.withgoogle.com/codejam/round/00000000008778ec

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

define run_test_exe_base_tgt_diff
.PHONY: ${3}

${3}: ${BINDIR}/${1} ${2}.in ${2}.out
	timeout 2 ${BINDIR}/${1} -naive ${2}.in ${2}.xnout
	$(if $(filter ${4},diffsimple),diff ${2}.xnout ${2}.out,@true)
	$(if $(filter ${4},diffposs),diff <(grep IMPOSSIBLE ${2}.xnout) <(grep IMPOSSIBLE ${2}.out),@true)
	timeout 2 ${BINDIR}/${1} ${2}.in ${2}.xout
	$(if $(filter ${4},diffsimple),diff ${2}.xout ${2}.out,@true)
	$(if $(filter ${4},diffposs),diff <(grep IMPOSSIBLE ${2}.xout) <(grep IMPOSSIBLE ${2}.out),@true)
endef

define run_test
$(eval $(call run_test_exe_base_tgt_diff,${1},${1}-tiny,${1}-test,diffsimple))
endef

define run_test_possible
$(call run_test_exe_base_tgt_diff,${1},${1}-tiny,${1}-test,diffposs)
endef

define run_test2
.PHONY: ${1}-test

${1}-test: ${1}-test1 ${1}-test2
$(call run_test_exe_base_tgt_diff,${1},${1}-tiny,${1}-test1,diffsimple)
$(call run_test_exe_base_tgt_diff,${1},${1}-tiny2,${1}-test2,diffsimple)
endef

# $(eval $(call run_test,xxx))
# $(eval $(call run_test2,xxx))
# $(eval $(call run_test,spiraling))
$(eval $(call run_test_possible,spiraling))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
