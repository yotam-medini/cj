ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

# ARCH:=$(shell arch)
# LIBDIR=/usr/lib/${ARCH}-linux-gnu
# LIBGMP=${LIBDIR}/libgmpxx.a ${LIBDIR}/libgmp.a

DBGFLAGS = -g -fno-inline
CFLAGS = ${DBGFLAGS} -Wall -Wshadow -std=c++17 -MMD

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

bbintree-stat-test: ${BINDIR}/bbintree-stat-test
	$< -special 7 0 1 2 5 6 3 4 2
	$< -special 7 0 1 2 4 5 6 3 4
	$< -special 5 0 1 2 3 4 1
	$< -special 6 0 1 2 3 4 5
	$< -special 3 0 1 2 0
	$< -special 4 0 1 3 2 3
	$< -special 4 0 1 3 2 1
	$< -special 4 0 1 2 3 2
	$< -special 4 0 1 2 3 1
	$< -special 3 0 1 2 1
	$< -special 3 0 1 2
	$< -special 3 1 0 2 0
	$< -special 2 0 1
	$< -special 1 0
	$< 1
	$< 2
	$< 3
	$< 4
	$< 5
	$< 6

bbintree-stat-test-long: ${BINDIR}/bbintree-stat-test
	$< 7

${BINDIR}/bbintree-stat-test: obj.d/bbintree-stat-test.o
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
whmatrix-test: ${BINDIR}/whmatrix-test
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
	${BINDIR}/comb choose 7 2
	${BINDIR}/comb tuples 2 3 2
	${BINDIR}/comb permutations 4
	${BINDIR}/comb combinations 6 3
	timeout 1 ${BINDIR}/comb multichoose 5 2
	timeout 1 ${BINDIR}/comb compositions 5 3

# Used to link with -lgmpxx -lgmp
${BINDIR}/comb: comb.cc
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -DTEST_COMB=1 -o $@ obj.d/comb.o


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

readline_ints-test: ${BINDIR}/readline_ints
	(echo 2; echo '22 3 '; echo ' 22 33 55'; echo ' 22 33 55 ';) | \
	     tee /tmp/readline_ints.inp | \
	     ${BINDIR}/readline_ints

${BINDIR}/readline_ints: obj.d/readline_ints.o
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -o $@ obj.d/readline_ints.o


${BINDIR}/interval_tree-test: obj.d/interval_tree-test.o
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -o $@ $<

.PHONY: interval_tree-test
interval_tree-test: ${BINDIR}/interval_tree-test
	$< add_remove 13  7  8 9  11 13  1 4  4 11  6 10  8 10  5 12  1  0
	$< add_remove 12  7  7 12  6 9  8 9  2 4  10 11  4 8  5 12  2  0 5
	$< add_remove 7  4  1 5  0 4  2 3  5 7  1  0
	$< add_remove 9  4  3 7  1 6  4 5  8 9  1  0
	$< add_remove 4  3  1 2  0 4  3 4  1 2
	$< add_remove 9  3  2 3  1 9  8 9   1  2
	$< add_remove 9  4  8 9  1 9  4 8  2 3  2  2 0
	$< add_remove 4  3  0 4  1 2  2 3  1  1
	$< add_remove 4  2  1 2  2 3  1  1
	$< random 1 9 5

${BINDIR}/segtree-test: obj.d/segtree.o
	@mkdir -p $(@D)
	g++ -g ${CFLAGS} -o $@ $<

.PHONY: segtree-test
segtree-test: ${BINDIR}/segtree-test
	${BINDIR}/segtree-test 42 1 10 20


${BINDIR}/nextbound: obj.d/nextbound.o
	@mkdir -p $(@D)
	g++ ${CFLAGS} -o $@ $^

.PHONY: nextbound-test
nextbound-test: ${BINDIR}/nextbound
	${BINDIR}/nextbound -special 3 0 1 2 3 4 5 6 7
	${BINDIR}/nextbound -special 4 0 1 2 3 4 5 6 7
	${BINDIR}/nextbound -special 5 0 1 2 3 4 5 6 7
	${BINDIR}/nextbound -special 4 0 1 2 3 4 0 0
	${BINDIR}/nextbound -special 0 1 1
	${BINDIR}/nextbound 190 1 32

${BINDIR}/primes: primes.cc
	@mkdir -p $(@D)
	g++ ${CFLAGS} -DTEST_PRIMES=1 -o $@ $^

.PHONY: primes-test
primes-test: ${BINDIR}/primes
	${BINDIR}/primes 0x20

${BINDIR}/dtos: obj.d/dtos.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: dtos-test
dtos-test: ${BINDIR}/dtos
	${BINDIR}/dtos 314159 31 31. \
	    31.4 31.41 31.415 31.4159 31.41592 31.415926 31.4159265

${BINDIR}/vlist: obj.d/vlist.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: vlist-test
vlist-test: ${BINDIR}/vlist
	${BINDIR}/vlist

${BINDIR}/skiplist: obj.d/skiplist.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: skiplist-test
skiplist-test: ${BINDIR}/skiplist
	timeout 2 ${BINDIR}/skiplist special i 10
	timeout 2 ${BINDIR}/skiplist special i 10 i 15
	timeout 2 ${BINDIR}/skiplist special i 10 i 5
	timeout 2 ${BINDIR}/skiplist special i 10 i 5 i 15
	timeout 2 ${BINDIR}/skiplist special i 10 r 10
	timeout 2 ${BINDIR}/skiplist special i 10 i 5 r 10

${BINDIR}/skiplistlist: obj.d/skiplistlist.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: skiplistlist-test
skiplistlist-test: ${BINDIR}/skiplistlist
	timeout 2 ${BINDIR}/skiplistlist special i 0 5 r 0 i 0 6 i 0 0 r 0 i 1 5 g 0
	timeout 2 ${BINDIR}/skiplistlist special i 0 10
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 g 0
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 r 0
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 i 1 15
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 i 1 15 i 2 20
ifeq (0,1)
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 i 0 5
	timeout 2 ${BINDIR}/skiplistlist special i 0 10 i 0 5 i 2 15
endif

${BINDIR}/n_near_pairs: obj.d/n_near_pairs.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: nearpairs-test
nearpairs-test: ${BINDIR}/n_near_pairs
	timeout 2 ${BINDIR}/n_near_pairs special 2 1 : 2 : 1 0
	timeout 2 ${BINDIR}/n_near_pairs special 1 : 0 : 0 0
	timeout 2 ${BINDIR}/n_near_pairs special 1 8 : 1 8 : 0 4
	timeout 2 ${BINDIR}/n_near_pairs 10 1 10 1 10 0 10 0 10 0 5 0 5

${BINDIR}/subsums: obj.d/subsums.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: subsums-test
subsums-test: ${BINDIR}/subsums
	timeout 2 ${BINDIR}/subsums 0 0
	timeout 2 ${BINDIR}/subsums 1 0
	timeout 2 ${BINDIR}/subsums 3 0 1
	timeout 2 ${BINDIR}/subsums 5 0 1 2 3 4 5 6
	timeout 2 ${BINDIR}/subsums 5 1 2 3 4 5 6
	timeout 2 ${BINDIR}/subsums 7 1 2 3
	timeout 2 ${BINDIR}/subsums 15 1 2 3 4
	timeout 2 ${BINDIR}/subsums 31 1 2 3 4 5
	timeout 2 ${BINDIR}/subsums 10 0 0 2 4

${BINDIR}/frac: obj.d/frac.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: frac-test
frac-test: ${BINDIR}/frac
	${BINDIR}/frac 1 + 1
	${BINDIR}/frac 1/2 + 1/3
	${BINDIR}/frac 1/2 - 1/3
	${BINDIR}/frac 1/3 - 1
	${BINDIR}/frac 5/12 + 7/12

${BINDIR}/bigfrac-test: bigfrac.cc
	g++ ${CFLAGS} -DTEST_BIG_FRAC=1 -o $@ $^

.PHONY: bigfrac-test
bigfrac-test: ${BINDIR}/bigfrac-test
	${BINDIR}/bigfrac-test test_choose 60 30
	${BINDIR}/bigfrac-test test_choose 120 60
	${BINDIR}/bigfrac-test test_double 3 4
	${BINDIR}/bigfrac-test test_double 10 70
	${BINDIR}/bigfrac-test test_frac 234500000 543 1234500000 5431

${BINDIR}/seg2d: obj.d/seg2d.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: seg2d-test
seg2d-test: ${BINDIR}/seg2d
	${BINDIR}/seg2d spec2d 3 2 3 6 7 5 3 5
	${BINDIR}/seg2d rand1d 4 1 1
	${BINDIR}/seg2d rand1d 2 2 4
	${BINDIR}/seg2d rand1d 10 3 6
	${BINDIR}/seg2d rand1d 20 4 9
	${BINDIR}/seg2d rand2d 4 1 1 4
	${BINDIR}/seg2d rand2d 20 2 2 9
	${BINDIR}/seg2d rand2d 100 3 2 9
	${BINDIR}/seg2d rand2d 100 2 3 9
	${BINDIR}/seg2d rand2d 100 4 3 9
	${BINDIR}/seg2d rand2d 100 3 4 9

${BINDIR}/ninv: obj.d/ninv.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: ninv-test
ninv-test: ${BINDIR}/ninv
	timeout 30 ${BINDIR}/ninv

${BINDIR}/bit: obj.d/bit.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: bit-test
bit-test: ${BINDIR}/bit
	timeout 4 ${BINDIR}/bit 10 6 20 9

${BINDIR}/bit2: obj.d/bit2.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: bit2-test
bit2-test: ${BINDIR}/bit2
	timeout 4 ${BINDIR}/bit2 specific 2 2 u 2 2 1 u 2 2 1 q 3 3
	timeout 4 ${BINDIR}/bit2 10 6 20 20 9

${BINDIR}/subsetsum: obj.d/subsetsum.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: subsetsum-test
subsetsum-test: ${BINDIR}/subsetsum
	timeout 4 ${BINDIR}/subsetsum 10 1 2 3 4 5
	timeout 4 ${BINDIR}/subsetsum 100 $$(seq 20)
	timeout 4 ${BINDIR}/subsetsum 10000 $$(seq 20)

${BINDIR}/cycledist: obj.d/cycledist.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: cycledist-test
cycledist-test: ${BINDIR}/cycledist
	timeout 4 ${BINDIR}/cycledist special 2 2 3 3 0 1
	timeout 4 ${BINDIR}/cycledist special 3 4 2 2 0 1
	timeout 4 ${BINDIR}/cycledist random 10 2 2 1 3
	timeout 4 ${BINDIR}/cycledist random 10 1 5 1 9

${BINDIR}/msb: obj.d/msb.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: msb-test
msb-test: ${BINDIR}/msb
	timeout 2 ${BINDIR}/msb

${BINDIR}/bigint: obj.d/bigint.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: bigint-test
bigint-test: ${BINDIR}/bigint
	timeout 4 ${BINDIR}/bigint s 1 2
	timeout 4 ${BINDIR}/bigint s 1 10
	timeout 4 ${BINDIR}/bigint s 1 16
	timeout 4 ${BINDIR}/bigint 16 5
	timeout 4 ${BINDIR}/bigint 4 3
	timeout 4 ${BINDIR}/bigint 3 -3
	timeout 4 ${BINDIR}/bigint 3 2
	timeout 4 ${BINDIR}/bigint 0 -1
	timeout 4 ${BINDIR}/bigint 1 1
	timeout 4 ${BINDIR}/bigint 2 2
	timeout 4 ${BINDIR}/bigint 2 1
	timeout 4 ${BINDIR}/bigint 1 2
	timeout 4 ${BINDIR}/bigint 8 7
	timeout 4 ${BINDIR}/bigint 9876 123
	timeout 4 ${BINDIR}/bigint 9876 345

${BINDIR}/disjset: obj.d/disjset.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: disjset-test
disjset-test: ${BINDIR}/disjset
	timeout 4 ${BINDIR}/disjset specific 1 5 4 -1 3 2 0 -1
	timeout 4 ${BINDIR}/disjset specific 1 2

${BINDIR}/invmod: obj.d/invmod.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: invmod-test
invmod-test: ${BINDIR}/invmod
	timeout 30 ${BINDIR}/invmod 1000000 1 2 3 4 5 6 7 8 0x10 0x100 0x100

${BINDIR}/nbitson: nbitson.cc
	g++ -DTEST_NBITSON=1 ${CFLAGS} -o $@ $^

.PHONY: nbitson-test
nbitson-test: ${BINDIR}/nbitson
	${BINDIR}/nbitson $$(seq 0 20) 0x100 0x200 0x3333 0xffff

${BINDIR}/sqroot: sqroot.cc
	g++ -DTEST_SQROOT=1 ${CFLAGS} -o $@ $^

.PHONY: sqroot-test
sqroot-test: ${BINDIR}/sqroot
	timeout 2 ${BINDIR}/sqroot $$(seq 0 26) \
	    0x1000000 0x10000000 0x1000000000000 0x1000000000001 0x1000000000002

${BINDIR}/extgcd: extgcd.cc
	g++ -DTEST_EXTGCD=1 ${CFLAGS} -o $@ $^

.PHONY: extgcd-test
extgcd-test: ${BINDIR}/extgcd
	timeout 10 ${BINDIR}/extgcd
	timeout 10 ${BINDIR}/extgcd invmod 1000000007 $$(seq 8) 1000000006


${BINDIR}/vpartition: vpartition.cc
	g++ -DTEST_VPARTITION=1 ${CFLAGS} -o $@ $^

.PHONY: vpartition-test
vpartition-test: ${BINDIR}/vpartition
	timeout 10 ${BINDIR}/vpartition 5  4 6 0 2
	timeout 10 ${BINDIR}/vpartition


${BINDIR}/maxprofit: maxprofit.cc
	g++ -DTEST_MAXPROFIT=1 ${CFLAGS} -o $@ $^

.PHONY: maxprofit-test
maxprofit-test: ${BINDIR}/maxprofit
	timeout 2 ${BINDIR}/maxprofit specific 2  2 4 8 6 7


${BINDIR}/isqrt: isqrt.cc
	g++ ${CFLAGS} -o $@ $^

.PHONY: isqrt-test
isqrt-test: ${BINDIR}/isqrt
	timeout 2 ${BINDIR}/isqrt $$(seq 0 20) 0xfffffff 0x7fffffff


${BINDIR}/graph: graph.cc
	g++ ${CFLAGS} -o $@ $^

.PHONY: graph-test
graph-test: ${BINDIR}/graph testdata.d/strong_components.in
	timeout 2 \
	  ${BINDIR}/graph strong_components testdata.d/strong_components.in

${BINDIR}/dfs: dfs.cc
	g++ ${CFLAGS} -o $@ $^

.PHONY: dfs-test
dfs-test: ${BINDIR}/dfs graph-dfs.in
	timeout 1 ${BINDIR}/dfs specific graph-dfs.in
	timeout 1 ${BINDIR}/dfs all 3
	timeout 1 ${BINDIR}/dfs all 4
	timeout 1 ${BINDIR}/dfs all 5
	timeout 1 ${BINDIR}/dfs random 2 3 6

${BINDIR}/modint: modint.cc
	g++ ${CFLAGS} -o $@ $^

.PHONY: modint-test
modint-test: ${BINDIR}/modint
	timeout 2 ${BINDIR}/modint 2 3

obj.d/factor-test.o: factor.cc
	g++ -c ${CFLAGS} -DTEST_FACTOR -o $@ $<

${BINDIR}/factor: obj.d/factor-test.o obj.d/primes.o obj.d/comb.o
	g++ ${CFLAGS} -o $@ $^

.PHONY: factor-test
factor-test: ${BINDIR}/factor
	timeout 2 ${BINDIR}/factor 7 18 24 25 36

${BINDIR}/hungarian: hungarian.cc
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -DTEST_HUNGARIAN -o $@ $^

.PHONY: hungarian-test-naive-max
hungarian-test-naive-max: ${BINDIR}/hungarian hungarian-clrs-weight.in
	timeout 2 ${BINDIR}/hungarian naive-max hungarian-clrs-weight.in

.PHONY: hungarian-test-max
hungarian-test-max: ${BINDIR}/hungarian hungarian-clrs-weight.in
	timeout 2 ${BINDIR}/hungarian max hungarian-clrs-weight.in

.PHONY: hungarian-test-max-compare
hungarian-test-max-compare: ${BINDIR}/hungarian hungarian-clrs-weight.in
	timeout 2 ${BINDIR}/hungarian max-compare hungarian-clrs-weight.in

${BINDIR}/kmp: kmp.cc
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -DTEST_KMP=1 -o $@ $^

.PHONY: kmp-test
kmp-test: ${BINDIR}/kmp
	timeout 2 ${BINDIR}/kmp specific ababaca bacbabababacacc
	timeout 2 ${BINDIR}/kmp specific ababaca bacbababaabcbab

${BINDIR}/suffix_array: obj.d/suffix_array.o
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -o $@ $^

.PHONY: suffix_array-test
suffix_array-test: ${BINDIR}/suffix_array
	timeout 2 ${BINDIR}/suffix_array abaab
	timeout 2 ${BINDIR}/suffix_array

${BINDIR}/rmq: obj.d/rmq.o
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -o $@ $^

.PHONY: rmq-test
rmq-test: ${BINDIR}/rmq
	timeout 2 ${BINDIR}/rmq specific 2 3 1 2 3 4
	timeout 2 ${BINDIR}/rmq 6 -1 3

${BINDIR}/ilog2: ilog2.cc
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -o $@ -DTEST_ILOG2=1 $<

.PHONY: ilog2-test
ilog2-test: ${BINDIR}/ilog2
	timeout 2 ${BINDIR}/ilog2 specific 0 $$(seq 20)


${BINDIR}/persistent_rbtree: persistent_rbtree.cc
	@mkdir -p ${BINDIR}
	g++ ${CFLAGS} -o $@ -DTEST_PERSISTENTRBTREE=1 $<

.PHONY: persistent_rbtree-test
# 	timeout 2 ${BINDIR}/persistent_rbtree specific 0 $$(seq 20)
persistent_rbtree-test: ${BINDIR}/persistent_rbtree
	timeout 2 ${BINDIR}/persistent_rbtree \
	    commands i 0 i 1 i 2 i 4 i 5 i 3 e 0
	timeout 2 ${BINDIR}/persistent_rbtree \
	    commands i 0 i 1 i 2 i 3 i 4 i 5 e 0
	timeout 2 ${BINDIR}/persistent_rbtree \
	    commands i 10 i 11 i 12 i 13 i 14 i 15 p e 11 p
	timeout 2 ${BINDIR}/persistent_rbtree commands i 0 p i 1 p i 2 p e 1 p
	timeout 2 ${BINDIR}/persistent_rbtree commands i 10 i 11 i 12 i 13 p e 10
	timeout 2 ${BINDIR}/persistent_rbtree commands i 10 i 11 i 12 i 13 p e 10 p e 12
	timeout 2 ${BINDIR}/persistent_rbtree specific 0 1 2 3 0 1 2 3
	timeout 2 ${BINDIR}/persistent_rbtree specific 0 2 1 0 1 2
	timeout 2 ${BINDIR}/persistent_rbtree specific 0 1 2 1 2 0
	timeout 2 ${BINDIR}/persistent_rbtree permutate 1 4


clean:
	rm -f ${OBJS} ${BINDIR}

-include ${DEPS}
