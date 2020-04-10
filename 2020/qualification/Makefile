#  Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 

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

${BINDIR}/vestigium: obj.d/vestigium.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

vestigium-test: ${BINDIR}/vestigium vestigium-tiny.in vestigium-tiny.out
	timeout 8 ${BINDIR}/vestigium -naive vestigium-tiny.in vestigium-tiny.xnout
	diff vestigium-tiny.xnout vestigium-tiny.out
	timeout 8 ${BINDIR}/vestigium vestigium-tiny.in vestigium-tiny.xout
	diff vestigium-tiny.xout vestigium-tiny.out

${BINDIR}/nestdepth: obj.d/nestdepth.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

nestdepth-test: ${BINDIR}/nestdepth nestdepth-tiny.in nestdepth-tiny.out
	timeout 8 ${BINDIR}/nestdepth -naive nestdepth-tiny.in nestdepth-tiny.xnout
	diff nestdepth-tiny.xnout nestdepth-tiny.out
	timeout 8 ${BINDIR}/nestdepth nestdepth-tiny.in nestdepth-tiny.xout
	diff nestdepth-tiny.xout nestdepth-tiny.out

${BINDIR}/parenpart: obj.d/parenpart.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

parenpart-test: ${BINDIR}/parenpart parenpart-tiny.in parenpart-tiny.out
	grep IMPOSSIBLE parenpart-tiny.out > parenpart-tiny.IMPOSSIBLE
	timeout 8 ${BINDIR}/parenpart -naive parenpart-tiny.in parenpart-tiny.xnout
	(grep IMPOSSIBLE parenpart-tiny.xnout; true) > parenpart-tiny.xnIMPOSSIBLE
	diff parenpart-tiny.xnIMPOSSIBLE parenpart-tiny.IMPOSSIBLE
	timeout 8 ${BINDIR}/parenpart parenpart-tiny.in parenpart-tiny.xout
	(grep IMPOSSIBLE parenpart-tiny.xout; true) > parenpart-tiny.xIMPOSSIBLE
	diff parenpart-tiny.xIMPOSSIBLE parenpart-tiny.IMPOSSIBLE

${BINDIR}/esab: obj.d/esab.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

esab-test-0: ${BINDIR}/esab interactive_runner.py my-local_testing_tool.py
	timeout 4 python interactive_runner.py \
	  python ./my-local_testing_tool.py 0 -- ./${BINDIR}/esab -debug 0x1

esab-test-1: ${BINDIR}/esab interactive_runner.py my-local_testing_tool.py
	timeout 4 python interactive_runner.py \
	  python ./my-local_testing_tool.py 1 -- ./${BINDIR}/esab -debug 0x1

esab-test-2: ${BINDIR}/esab interactive_runner.py my-local_testing_tool.py
	timeout 30 python interactive_runner.py \
	  python ./my-local_testing_tool.py 2 -- ./${BINDIR}/esab -debug 0x1

esab-test: esab-test-0 esab-test-1 esab-test-2

${BINDIR}/indicium: obj.d/indicium.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

indicium-test: ${BINDIR}/indicium indicium-tiny.in indicium-tiny.out
	grep IMPOSSIBLE indicium-tiny.out > indicium-tiny.IMPOSSIBLE
	timeout 8 ${BINDIR}/indicium -naive indicium-tiny.in indicium-tiny.xnout
	(grep IMPOSSIBLE indicium-tiny.xnout; true) > indicium-tiny.xnIMPOSSIBLE
	diff indicium-tiny.xnIMPOSSIBLE indicium-tiny.IMPOSSIBLE
	timeout 8 ${BINDIR}/indicium indicium-tiny.in indicium-tiny.xout
	(grep IMPOSSIBLE indicium-tiny.xnout; true) > indicium-tiny.xIMPOSSIBLE
	diff indicium-tiny.xIMPOSSIBLE indicium-tiny.IMPOSSIBLE

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
