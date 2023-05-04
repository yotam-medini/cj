# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
# codingcompetitions.withgoogle.com/codejam/round/00000000008778ec

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

$(eval $(call run_test_possible,spiraling))
$(eval $(call run_test,pixelated))
$(eval $(call run_test_possible,jelly))
$(eval $(call run_test,iobot))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
