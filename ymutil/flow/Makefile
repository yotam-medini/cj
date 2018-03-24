ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

bmaxflow-test: /tmp/edmonds-karp-eg max_flow.dat max_flow-tiny.dat
	/tmp/edmonds-karp-eg < max_flow-tiny.dat
	/tmp/edmonds-karp-eg < max_flow.dat

MAXFLOW_SRC = \
	Makefile \
	maxflow.h \
	maxflow.cc

/tmp/maxflow.tgz: ${MAXFLOW_SRC}
	tar czf $@ ${MAXFLOW_SRC}
	tar tvzf $@; ls -lG $@

/tmp/edmonds-karp-eg: edmonds-karp-eg.cpp
	g++ -g -Wall -o $@ $<

obj.d/%.o: %.cc
	@mkdir -p ${@D}
	g++ -c -g -Wall -Wshadow --std=c++11 -fno-exceptions -MMD -o $@ $<

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))


# /tmp/maxflow-test: maxflow-test.cc maxflow.cc maxflow.h
MFT_OBJS = \
	obj.d/maxflow.o \
	obj.d/maxflow-test.o

/tmp/maxflow-test: ${MFT_OBJS}
	g++ -g -o $@ ${MFT_OBJS}

maxflow-test: /tmp/maxflow-test max_flow.dat max_flow-tiny.dat
	/tmp/maxflow-test < max_flow-tiny.dat
	/tmp/maxflow-test < max_flow.dat

BIP_OBJS = \
	obj.d/bipartite-test.o \
	obj.d/bipartite_match.o \
	obj.d/maxflow.o

/tmp/bipartite-test: ${BIP_OBJS}
	g++ -g -o $@ ${BIP_OBJS}

bipartite-test: /tmp/bipartite-test
	/tmp/bipartite-test 10 10  20 10  20 30  30 30  30 40  40 30  50 30

clean:
	rm -f ${OBJS} /tmp/edmonds-karp-eg

-include ${DEPS}
