#!/usr/bin/env python3

import pprint
from random import randint
import sys

def vlog(msg):
    sys.stderr.write('%s\n' % msg)

ow = sys.stdout.write

ai = 1
N = int(sys.argv[ai]); ai += 1
S = int(sys.argv[ai]); ai ++ 1

junctions = list(range(1, N + 1))
for k in range(N):
    r = randint(k, N - 1)
    t = junctions[k]
    junctions[k] = junctions[r]
    junctions[r] = t

jt0 = N // 3
jt1 = 2*jt0
jt2 = N - 1
streets = []
for (b, e) in [(1, jt0), (jt0 + 1, jt1), (jt1 + 1, jt2)]:
    k = b
    streets.append((junctions[0], junctions[k]))
    while streets[-1][1] != junctions[e]:
        k += 1
        street = (streets[-1][1], junctions[k])
        streets.append(street)

stones = list(range(S + 1))
for k in range(2, S - 1):
    r = randint(k, S - 1)
    t = stones[k]
    stones[k] = stones[r]
    stones[r] = t
vlog('stones=%s' % str(stones))

jstones = N * [[]]
vlog('jstones=%s' % str(jstones))
jstones[jt0] = [stones[2]]
jstones[jt1] = [stones[3]]
jstones[jt2] = [stones[4]]

irecipes = []

irecipes.append([2, 2, 3, 5])
irecipes.append([2, 3, 4, 6])
irecipes.append([2, 2, 4, 7])

irecipes.append([2, 4, 5, 8])
irecipes.append([2, 2, 6, 9])
irecipes.append([2, 3, 7, 10])

j3b = len(irecipes)
irecipes.append([3, 2, 3, 8, 11])
irecipes.append([3, 3, 4, 9, 12])
irecipes.append([3, 2, 4, 7, 13])

while irecipes[-1][-1] < S - 3:
    j3b_next = j3b + 3
    for j in (0, 1, 2):
        ings_old2 = irecipes[j3b + j][1: 3]
        ing0 = (2 + 3 + 4) - (ings_old2[0] + ings_old2[1])
        ing1 = ings_old2[randint(0, 1)]
        ing2 = irecipes[j3b + j][-1]
        irecipe = [3, ing0, ing1, ing2, irecipes[-1][-1] + 1]
        irecipes.append(irecipe)
    j3b = j3b_next
gold_recipe = [3, irecipes[-1][-1], irecipes[-2][-1], irecipes[-3][-1], 1]
irecipes.append(gold_recipe)

vlog('S=%d, irecipes=%s' % (S, pprint.pformat(irecipes)))
recipes = []
for irecipe in irecipes:
    n_ing = irecipe[0]
    ings = []
    for i in irecipe[1:-1]:
        ings.append(stones[i])
    ings.sort()
    recipe = [irecipe[0]] + ings + [stones[irecipe[-1]]]
    recipes.append(recipe)

# permutate
NR = len(recipes)
for k in range(NR):
    r = randint(k, NR - 1)
    trecipe = recipes[k]
    recipes[k] = recipes[r]
    recipes[r] = trecipe
vlog('S=%d, recipes=\n%s' % (S, pprint.pformat(recipes)))

ow('1\n%d %d %d %d\n' % (N, len(streets), S, len(recipes)))
for street in streets:
    ow('%d %d\n' % street)
for js in jstones:
    ow('%d' % len(js))
    for s in js:
        ow(' %d' % s)
    ow('\n')

for recipe in recipes:
    ow('%s\n' % (' '.join(map(lambda n: '%d' % n, recipe))))
    
sys.exit(0)
 
