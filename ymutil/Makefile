ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

ARCH:=$(shell arch)
LIBDIR=/usr/lib/${ARCH}-linux-gnu
LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++11 -MMD

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

${BINDIR}/choose: obj.d/choose.o
	@mkdir -p $(@D)
	g++ -g -o $@ $< ${LIBGMP}

${BINDIR}/matrix-test: obj.d/matrix-test.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<


${BINDIR}/whmatrix-test: obj.d/whmatrix-test.o
	@mkdir -p $(@D)
	g++ -g -o $@ $<


.PHONY: choose-test
choose-test: ${BINDIR}/choose
	$<

.PHONY: matrix-test whmatrix-test
matrix-test: ${BINDIR}/matrix-test
	$<

LINEQ_TEST_OBJS = \
	obj.d/lineq-test.o \
	obj.d/lineq.o

${BINDIR}/lineq-test: ${LINEQ_TEST_OBJS}
	@mkdir -p $(@D)
	g++ -g -o $@ ${LINEQ_TEST_OBJS}

.PHONY: lineq-test
lineq-test: ${BINDIR}/lineq-test
	${BINDIR}/lineq-test  2  1 0 0 1  2 3
	${BINDIR}/lineq-test  2  0 1 1 0  2 3
	${BINDIR}/lineq-test  2  1 2 3 4  5 6
	${BINDIR}/lineq-test  3  1 3 -2  3 5 6  2 4 3  5 7 8

.PHONY: comb-test
comb-test: ${BINDIR}/comb
	${BINDIR}/comb

${BINDIR}/comb: obj.d/comb.o
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -o $@ obj.d/comb.o -lgmpxx -lgmp


.PHONY: modinv-test
modinv-test: ${BINDIR}/modinv
	${BINDIR}/modinv 2
	${BINDIR}/modinv 3
	${BINDIR}/modinv 5
	${BINDIR}/modinv 7
	${BINDIR}/modinv 11
	${BINDIR}/modinv 13

${BINDIR}/modinv: obj.d/modinv.o
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -o $@ obj.d/modinv.o

clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
