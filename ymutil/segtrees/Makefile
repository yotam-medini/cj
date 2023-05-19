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
	${BINDIR}/sum_query specific 2 0 1 0 7 0 4
	${BINDIR}/sum_query random 123  1 10  5  0 20


${BINDIR}/range_update: range_update.cc
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $<
	ls -lGt $@

range_update-test: ${BINDIR}/range_update
	${BINDIR}/range_update specific 2 0 1 0 7 0 4
	${BINDIR}/range_update random 123  1 10  5  0 20


clean:
	rm -f ${OBJS}
	rm -rf ${BINDIR}

-include ${DEPS}
