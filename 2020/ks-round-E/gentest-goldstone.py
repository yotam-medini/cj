#!/usr/bin/env python3

import os
from random import randint
import sys

progname = 'goldstone'

def ew(msg):
    sys.stderr.write('%s\n' % msg)

def syscmd(cmd):
    ew(cmd)
    rc = os.system(cmd)
    return rc

def check_rc(rc):
    if rc != 0:
        ew('Failed running %s' % progname)
        sys.exit(rc | (rc >> 8))
    
def rundiff(fn_in):
    fn_out_naive = '%s-auto-naive.out' % progname
    fn_out = '%s-auto.out' % progname
    rc1 = syscmd('./bin/%s -naive %s %s' % (progname, fn_in, fn_out_naive))
    rc2 = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
    check_rc(rc1 | rc2)
    rcdiff = syscmd('diff %s %s' % (fn_out_naive, fn_out))
    if rcdiff != 0:
        ew('Inconsistent')
        sys.exit(1)

def gen_graph(N):

    class Junction:
        def __init__(self, rep=None, component=None):
            self.rep = rep
            self.component = component
    streets = []
    # junctions = (N + 1)*[(None, None)]
    junctions = [Junction()]
    for j in range(1, N + 1):
        junctions.append(Junction(j, None))
    all_streets = []
    all_streets_set = set()
    for j in range(1, N + 1):
        for j1 in range(j + 1, N + 1):
            all_streets.append((j, j1))
            all_streets_set.add((j, j1))
    n_sets = 0 # #(components)
    assigned = set()
    loops = 0
    while (n_sets != 1) or (len(assigned) < N):
        # ew('loops: %d, n_sets=%d, #assigned=%d' %
        #    (loops, n_sets, len(assigned)))
        loops += 1
        for j in range(1, N + 1):
            junc = junctions[j]
            # ew('Junction[%d]: rep=%s, comp: %s' %
            #    (j, junc.rep, str(junc.component)))
        si = randint(0, len(all_streets) - 1)
        street = all_streets[si]
        # ew('street=%s' % str(street))
        streets.append(street)
        all_streets[si] = all_streets[-1]
        all_streets.pop()
        all_streets_set.remove(street)
        (j0, j1) = street
        jmin = min(junctions[j0].rep, junctions[j1].rep)
        # ew('jmin=%d' % jmin)
        if junctions[jmin].component is None:
            junctions[jmin].component = set()
            n_sets += 1
        for jx in (j0, j1):
            junctions[jmin].component.add(jx)
            jxcomp = junctions[junctions[jx].rep].component
            if jxcomp is not None:
                for j in jxcomp:
                    junctions[jmin].component.add(j)
            assigned.add(jx)
        for j in junctions[jmin].component - set([jmin]):
            junctions[j].rep = jmin
            if junctions[j].component is not None:
                n_sets -= 1
                junctions[j].component = None
    return streets

def choose_rep_rand(k, lfrom):
    ret = []
    while len(ret) < k:
        i = randint(0, len(lfrom) - 1)
        ret.append(lfrom[i])
    ret.sort()
    return ret

def choose_rand(k, lfrom):
    ret = []
    while len(ret) < k:
        i = randint(0, len(lfrom) - 1)
        ret.append(lfrom[i])
        lfrom[i] = lfrom[-1]
        lfrom.pop()
    ret.sort()
    return ret

def rand_jstones(N, S, SJmax, pre_stones):
    jstones = [None]
    for j in range(1, N + 1):
        jstones.append(set())
    for pre_stone in pre_stones:
        nj = randint(1, SJmax)
        juncs = choose_rand(nj, list(range(1, N)))
        for junc in juncs:
            jstones[junc].add(pre_stone)
    return jstones
    

def rand_n_ingredients():
    n_ing_selection = [1, 2, 2, 3, 3, 3]
    return n_ing_selection[randint(0, len(n_ing_selection) - 1)]
    
def get_recipes(S, Spre):
    pre_stones = choose_rand(Spre, list(range(2, S + 1)))
    pre_stones_set = set(pre_stones)
    recipes = []
    produced = set(pre_stones)
    size_toadd1 = randint(len(produced), S - 1)
    ing_pick_from = pre_stones[:]
    step = 0
    while (1 not in produced) or (len(produced) < (3*S)//4):
        step += 1
        if (step & (step - 1)) == 0:
            ew('step=%d' % step)
        missing = set(range(2, S + 1)) - produced
        if len(produced) >= size_toadd1:
            missing.add(1)
        result_pallete = list(produced) + step*list(missing)
        result = result_pallete[randint(0, len(result_pallete) - 1)]
        ing_pallete = list(produced - {result})
        # n_ings = randint(1, min(3, len(ing_pallete)))
        n_ings = randint(1, 3)
        ings = choose_rep_rand(n_ings, ing_pallete)
        recipe = [n_ings] + ings + [result]
        # ew('produced=%s, missing=%s' % (str(produced), str(missing)))
        # ew('recipe: %s' % str(recipe))
        if recipe not in recipes:
            produced.add(result)
            recipes.append(recipe)
    return pre_stones, recipes

def OLD_get_recipes(S, Spre):
    pre_stones = choose_rand(Spre, list(range(2, S + 1)))
    pre_stones_set = set(pre_stones)
    recipes = []
    for i in range(randint(1, 2)):
        n_ing = rand_n_ingredients()
        ings = choose_rep_rand(n_ing, list(range(2, S + 1)))
        recipe = [n_ing] + ings + [1]
        if recipe not in recipes:
            recipes.append(recipe)
            ew('G recipe: %s' % str(recipe))
        pending = set(ings) - pre_stones_set
    supplied = set()
    while len(pending) > 0:
        # ew('pending: %s, pre_stones=%s' % (str(pending), str(pre_stones)))
        p = list(pending)[0]
        n_ing = rand_n_ingredients()
        candidates = list(set(range(2, S + 1)) - pending)
        ings = choose_rep_rand(n_ing, candidates)
        recipe = [n_ing] + ings + [p]
        # ew('recipe: %s, candidates=%s' % (str(recipe), str(candidates)))
        if recipe not in recipes:
            # ew('append recipe: %s, pending=%s' % (str(recipe), str(pending)))
            recipes.append(recipe)
            pending.remove(p)
            supplied.add(p)
            # ew('remove(%d), pending=%s' % (p, str(pending)))
            need = set(ings) - pre_stones_set - supplied
            # ew('pending=%s, need=%s' % (str(pending), str(need)))
            pending = set.union(pending, need)
    return pre_stones, recipes

if __name__ == '__main__':
    large = False
    fn_in = '%s-auto.in' % progname
    ai = 1
    T = int(sys.argv[ai]); ai += 1
    Nmin = int(sys.argv[ai]); ai += 1
    Nmax = int(sys.argv[ai]); ai += 1
    Smin = int(sys.argv[ai]); ai += 1
    Smax = int(sys.argv[ai]); ai += 1
    Spre_min = int(sys.argv[ai]); ai += 1
    Spre_max = int(sys.argv[ai]); ai += 1
    SJmax = int(sys.argv[ai]); ai += 1 # max number of junction having stone
    for t in range(T):
        ew('Tested %d/%d' % (t, T))
        N = randint(Nmin, Nmax)
        streets = gen_graph(N)
        S = randint(Smin, Smax)
        Spre = randint(min(Spre_min, S - 1), min(Spre_max, S - 1))
        pre_stones, recipes = get_recipes(S, Spre)
        f = open(fn_in, 'w')
        f.write('1\n%d %d %d %d\n' % (N, len(streets), S, len(recipes)))
        for street in streets:
            f.write('%d %d\n' % street)
        jstones = rand_jstones(N, S, SJmax, pre_stones)
        for js in jstones[1:]:
            f.write('%d' % len(js))
            for pre_stone in js:
                f.write(' %d' % pre_stone)
            f.write('\n')
        for recipe in recipes:
            f.write('%s\n' % (' '.join(map(lambda n: '%d' % n, recipe))))
        f.close()
        if large:
            fn_out = '%s-auto.out' % progname
            rc = syscmd('./bin/%s %s %s' % (progname, fn_in, fn_out))
            check_rc(rc)
        else:
            rundiff(fn_in)
    sys.exit(0)
