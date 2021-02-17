#!/usr/bin/env python3
# Generate small goldstone input was huge result

import pprint
from random import randint
import sys

def vlog(msg):
    sys.stderr.write('%s\n' % msg)

ow = sys.stdout.write

ai = 1
S0 = int(sys.argv[ai]); ai ++ 1

recipes = []
last = [2, 3]
while last[-1] < S0:
    [s0, s1] = last
    next0, next1 = s0 + 2, s1 + 2
    recipe = [3, s0, s0, s1, next0]
    recipes.append(recipe)
    recipe = [3, s0, s1, s1, next1]
    recipes.append(recipe)
    last = [next0, next1]
grecipe = [2, last[0], last[1], 1]
recipes.append(grecipe)

ow('1\n')
ow('2 1 %d %d\n' % (last[1], len(recipes)))
ow('1 2\n') # single street
ow('1 2\n') # stone 2 @ junction 1
ow('1 3\n') # stone 3 @ junction 2
for recipe in recipes:
    ow('%s\n' % (' '.join(list(map(str, recipe)))))

sys.exit(0)
