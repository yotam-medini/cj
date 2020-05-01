ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

bmaxflow-boost-test: /tmp/edmonds-karp-eg max_flow.dat max_flow-tiny.dat
	/tmp/edmonds-karp-eg < max_flow-tiny.dat
	/tmp/edmonds-karp-eg < max_flow.dat

MAXFLOW-BOOST_SRC = \
	Makefile \
	maxflow-boost.h \
	maxflow-boost.cc

/tmp/maxflow-boost.tgz: ${MAXFLOW-BOOST_SRC}
	tar czf $@ ${MAXFLOW-BOOST_SRC}
	tar tvzf $@; ls -lG $@

/tmp/edmonds-karp-eg: edmonds-karp-eg.cpp
	g++ -g -Wall -o $@ $<

obj.d/%.o: %.cc
	@mkdir -p ${@D}
	g++ -c -g -Wall -Wshadow --std=c++11 -fno-exceptions -MMD -o $@ $<

CXXS := $(wildcard *.cc)
OBJS = $(patsubst %.cc,obj.d/%.o,$(CXXS))
DEPS = $(patsubst %.cc,obj.d/%.d,$(CXXS))


# /tmp/maxflow-boost-test: maxflow-boost-test.cc maxflow-boost.cc maxflow-boost.h
MFT_BOOST_OBJS = \
	obj.d/maxflow-boost.o \
	obj.d/maxflow-boost-test.o

/tmp/maxflow-boost-test: ${MFT_BOOST_OBJS}
	g++ -g -o $@ ${MFT_BOOST_OBJS}

maxflow-boost-test: /tmp/maxflow-boost-test max_flow.dat max_flow-tiny.dat
	/tmp/maxflow-boost-test < max_flow-tiny.dat
	/tmp/maxflow-boost-test < max_flow.dat

MFT_OBJS = \
	obj.d/maxflow.o \
	obj.d/maxflow-test.o

/tmp/maxflow-test: ${MFT_OBJS}
	g++ -g -o $@ ${MFT_OBJS}

maxflow-test: /tmp/maxflow-test max_flow.dat max_flow-tiny.dat
	timeout 4 /tmp/maxflow-test < max_flow-tiny.dat
	timeout 4 /tmp/maxflow-test < max_flow.dat

BIP_OBJS = \
	obj.d/bipartite-test.o \
	obj.d/bipartite_match.o \
	obj.d/maxflow.o

/tmp/bipartite-test: ${BIP_OBJS}
	g++ -g -o $@ ${BIP_OBJS}

bipartite-test: /tmp/bipartite-test
	/tmp/bipartite-test 10 10  20 10  20 30  30 30  30 40  40 30  50 30
	/tmp/bipartite-test \
	  0 2  0 3  0 4  1 2  1 3  1 4  3 1  3 2  3 4  4 1  4 2  4 3

clean:
	rm -f ${OBJS} /tmp/edmonds-karp-eg

-include ${DEPS}
