ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++11 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

bbintree-test: ${BINDIR}/bbintree-test
	$<

${BINDIR}/bbintree-test: obj.d/bbintree-test.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
