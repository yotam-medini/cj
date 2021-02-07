// BIT Binary Indexed 2D Tree
// Inspired by:
// topcoder.com/community/competitive-programming/tutorials/binary-indexed-trees
#include <vector>
using namespace std;
typedef unsigned u_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;

class BIT2
{
 public:
    BIT2(u_t _max_idx_x, u_t _max_idx_y) :
        max_idx_x(_max_idx_x),
        max_idx_y(_max_idx_y),
        tree((_max_idx_x + 1)*(_max_idx_y + 1), 0) {}
    void update(int idx_x, int idx_y, ull_t delta)
    {
        if ((idx_x > 0) && (idx_y > 0))
        {
            while (idx_x <= int(max_idx_x))
            {
                int y = idx_y;
                while (y <= int(max_idx_y))
                {
                    tree[ixy(idx_x, y)] += delta;
                    y += (y & -y);
                }
                idx_x += (idx_x & -idx_x);
            }
        }
    }
    ull_t query(int idx_x, int idx_y) const
    {
        ull_t n = 0;
        idx_x = min<int>(idx_x, max_idx_x);
        idx_y = min<int>(idx_y, max_idx_y);
        while (idx_x > 0)
        {
            int y = idx_y;
            while (y > 0)
            {
                n += tree[ixy(idx_x, y)];
                y -= (y & -y); // removing low bit
            }
            idx_x -= (idx_x & -idx_x); // removing low bit
        }
        return n;
    }
 private:
    u_t ixy(u_t x, u_t y) const { return x*(max_idx_y + 1) + y; }
    u_t max_idx_x;
    u_t max_idx_y;
    vull_t tree;
};

#include <numeric>

class Naive
{
 public:
    Naive(u_t _max_idx_x, u_t _max_idx_y) : 
        max_idx_x(_max_idx_x), 
        max_idx_y(_max_idx_y), 
        a(_max_idx_x + 1, vull_t(_max_idx_y + 1, 0)) {}
    void update(u_t idx_x, u_t idx_y, u_t delta)
    {
        a[idx_x][idx_y] += delta;
    }
    ull_t query(u_t idx_x, u_t idx_y) const
    {
        ull_t n = 0;
        const u_t ye = min<u_t>(idx_y, max_idx_y);
        for (u_t x = 0, xe = min<u_t>(idx_x, max_idx_x); x <= xe; ++x)
        {
            n = accumulate(a[x].begin(), a[x].begin() + ye + 1, n);
        }
        return n;
    }
 private:
    u_t max_idx_x;
    u_t max_idx_y;
    vvull_t a;
};

class Cmd
{
 public:
   Cmd() : update(false), idx_x(0), idx_y(0), delta(0) {}
   bool update;
   u_t idx_x, idx_y, delta;
};
typedef vector<Cmd> vcmd_t;

#include <iostream>
int test(u_t max_idx_x, int max_idx_y, const vcmd_t& cmds)
{
    BIT2 bit(max_idx_x, max_idx_y);
    Naive naive(max_idx_x, max_idx_y);
    int rc = 0;
    for (size_t ci = 0; (rc == 0) && (ci < cmds.size()); ++ci)
    {
        const Cmd& cmd = cmds[ci];
        if (cmd.update)
        {
            bit.update(cmd.idx_x, cmd.idx_y, cmd.delta);
            naive.update(cmd.idx_x, cmd.idx_y, cmd.delta);
        }
        else
        {
            u_t n = bit.query(cmd.idx_x, cmd.idx_y);
            u_t n_naive = naive.query(cmd.idx_x, cmd.idx_y);
            if (n != n_naive)
            {
                rc = 1;
                cerr << "n=" << n << " != n_naive=" << n_naive << "\n" <<
                    "specific " << max_idx_x << ' ' << max_idx_y;
                for (size_t eci = 0; eci <= ci; ++eci)
                {
                    const Cmd& ecmd = cmds[eci];
                    if (ecmd.update)
                    {
                        cerr << " u " << ecmd.idx_x << ' ' << ecmd.idx_y <<
                            ' ' << ecmd.delta;
                    }
                    else
                    {
                        cerr << " q" << ' ' << ecmd.idx_x << ' ' << ecmd.idx_y;
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
    u_t max_idx_x = stoi(argv[ai++]);
    u_t max_idx_y = stoi(argv[ai++]);
    vcmd_t cmds;
    while (ai < argc)
    {
        Cmd cmd;
        cmd.update = argv[ai++][0] == 'u';
        cmd.idx_x = stoi(argv[ai++]);
        cmd.idx_y = stoi(argv[ai++]);
        if (cmd.update)
        {
            cmd.delta = stoi(argv[ai++]);
        }
        cmds.push_back(cmd);
    }
    return test(max_idx_x, max_idx_y, cmds);    
}

int test_random(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    u_t nt = stoi(argv[ai++]);
    size_t n_cmds = stoi(argv[ai++]);
    u_t max_idx_x = stoi(argv[ai++]);
    u_t max_idx_y = stoi(argv[ai++]);
    u_t max_delta = stoi(argv[ai++]);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        vcmd_t cmds;
        while (cmds.size() < n_cmds)
        {
            Cmd cmd;
            cmd.update = (cmds.size() + 1 < n_cmds) && ((rand() % 2) == 0);
            cmd.idx_x = (rand() % (max_idx_x)) + 1;
            cmd.idx_y = (rand() % (max_idx_y)) + 1;
            if (cmd.update)
            {
                cmd.delta = rand() % (max_delta + 1);
            }
            cmds.push_back(cmd);
        }
        rc = test(max_idx_x, max_idx_y, cmds);
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
