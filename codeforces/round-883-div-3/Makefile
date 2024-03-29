# Author:  Yotam Medini  yotam.medini@gmail.com -- Created:
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

# parameters: 1: base executable 2: base-case, 3: target, 4: diff
define run_test_1234
.PHONY: ${3}

${3}: ${BINDIR}/${1} ${2}.in ${2}.out
	timeout 2 ${BINDIR}/${1} -naive ${2}.in ${2}.xnout
	${4} ${2}.xnout ${2}.out
	timeout 2 ${BINDIR}/${1} ${2}.in ${2}.xout
	${4} ${2}.xout ${2}.out
endef

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

NDIFF=numdiff -r 0.000001
define run_nd_test
.PHONY: ${1}-test
${1}-test: 
$(eval $(call run_test_1234,${1},${1}-tiny,${1}-test,${NDIFF}))
endef

define run_nd_test2
.PHONY: ${1}-test

${1}-test: ${1}-test1 ${1}-test2
$(call run_test_1234,${1},${1}-tiny,${1}-test1,${NDIFF})
$(call run_test_1234,${1},${1}-tiny2,${1}-test2,${NDIFF)
endef

# $(eval $(call run_test,xxx))
# $(eval $(call run_test2,xxx))
$(eval $(call run_test,a-rudolph))
$(eval $(call run_test,btictacto))
$(eval $(call run_test,ccomp))
$(eval $(call run_nd_test,dtree))

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
