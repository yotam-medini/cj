ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++17 -MMD

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))
BINDIR = bin

obj.d/%.o: %.cc
	@mkdir -p $(@D)
	g++ -c ${CFLAGS} -o $@ $<

${BINDIR}/sum_query: sum_query.cc
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $<
	ls -lGt $@

sum_query-test: ${BINDIR}/sum_query
	${BINDIR}/sum_query random 1  1 10  5 20


clean:
	rm -f ${OBJS}
	rm -rf ${BINDIR}

-include ${DEPS}
