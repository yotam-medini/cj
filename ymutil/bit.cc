// BIT Binary Indexed Tree
// Inspired by:
// topcoder.com/community/competitive-programming/tutorials/binary-indexed-trees
#include <vector>
using namespace std;
typedef unsigned u_t;
typedef vector<u_t> vu_t;

class BIT
{
 public:
    BIT(u_t _max_idx) : max_idx(_max_idx), tree(_max_idx + 1, 0) {}
    void update(u_t idx, u_t delta)
    {
        if (idx > 0)
        {
            while (idx <= max_idx)
            {
                tree[idx] += delta;
                idx += (idx & -idx);
            }
        }
    }
    u_t query(int idx) const
    {
        u_t n = 0;
        while (idx > 0)
        {
            n += tree[idx];
            idx -= (idx & -idx); // removing low bit
        }
        return n;
    }
 private:
    u_t max_idx;
    vu_t tree;
};

#include <numeric>

class Naive
{
 public:
    Naive(u_t _max_idx) : max_idx(_max_idx), a(_max_idx + 1, 0) {}
    void update(u_t idx, u_t delta)
    {
        a[idx] += delta;
    }
    u_t query(int idx) const
    {
        vu_t::const_iterator e = a.begin() + min<u_t>(max_idx, idx) + 1;
        u_t n = accumulate(a.begin(), e, 0);
        return n;
    }
 private:
    u_t max_idx;
    vu_t a;
    
};

class Cmd
{
 public:
   Cmd() : update(false), idx(0), delta(0) {}
   bool update;
   u_t idx, delta;
};
typedef vector<Cmd> vcmd_t;

#include <iostream>
int test(u_t max_idx, const vcmd_t& cmds)
{
    BIT bit(max_idx);
    Naive naive(max_idx);
    int rc = 0;
    for (size_t ci = 0; (rc == 0) && (ci < cmds.size()); ++ci)
    {
        const Cmd& cmd = cmds[ci];
        if (cmd.update)
        {
            bit.update(cmd.idx, cmd.delta);
            naive.update(cmd.idx, cmd.delta);
        }
        else
        {
            u_t n = bit.query(cmd.idx);
            u_t n_naive = naive.query(cmd.idx);
            if (n != n_naive)
            {
                rc = 1;
                cerr << "n=" << n << " != n_naive=" << n_naive << "\n" <<
                    "specific " << max_idx;
                for (size_t eci = 0; eci <= ci; ++eci)
                {
                    const Cmd& ecmd = cmds[eci];
                    if (ecmd.update)
                    {
                        cerr << " u " << ecmd.idx << ' ' << ecmd.delta;
                    }
                    else
                    {
                        cerr << " q" << ecmd.idx;
                    }
                }
                cerr << '\n';
            }
        }
    }
    return rc;
}

int test_specific(int argc, char** argv)
{
    int ai = 0;
    u_t max_idx = stoi(argv[ai++]);
    vcmd_t cmds;
    while (ai < argc)
    {
        Cmd cmd;
        cmd.update = argv[ai++][0] == 'u';
        cmd.idx = stoi(argv[ai++]);
        if (cmd.update)
        {
            cmd.delta = stoi(argv[ai++]);
        }
        cmds.push_back(cmd);
    }
    return test(max_idx, cmds);    
}

int test_random(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    u_t nt = stoi(argv[ai++]);
    size_t n_cmds = stoi(argv[ai++]);
    u_t max_idx = stoi(argv[ai++]);
    u_t max_delta = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vcmd_t cmds;
        while (cmds.size() < n_cmds)
        {
            Cmd cmd;
            cmd.update = (cmds.size() + 1 < n_cmds) && ((rand() % 2) == 0);
            cmd.idx = (rand() % (max_idx)) + 1;
            if (cmd.update)
            {
                cmd.delta = rand() % (max_delta + 1);
            }
            cmds.push_back(cmd);
        }
        rc = test(max_idx, cmds);
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (string(argv[1]) == string("specific"))
    {
        rc = test_specific(argc - 2, argv + 2);
    }
    else
    {
        rc = test_random(argc - 1, argv + 1);
    }
    return rc;
}
