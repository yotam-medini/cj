#include <iostream>
#include <map>

using namespace std;

typedef unsigned u_t;
typedef map<u_t, u_t> u2u_t;

class Intervals
{
 public:
    u_t add(u_t at, bool up);
    u_t size() const { return b2e.size(); }
    const u2u_t& get_map() const { return b2e; }
 private:
    typedef u2u_t::iterator iter_t;
    typedef u2u_t::reverse_iterator riter_t;
    typedef u2u_t::value_type valt_t;
    u_t extend(iter_t iter, bool up);
    u2u_t b2e; // [begin, end)
};

u_t Intervals::add(u_t at, bool up)
{
    u_t ret = at;
    if (b2e.empty())
    {
        b2e.insert(b2e.end(), valt_t(at, at + 1));
    }
    else
    {
        auto er = b2e.equal_range(at);
        iter_t iter = er.first;
        if (er.first != er.second)
        {
            ret = extend(iter, up);
        }
        else if ((er.first != b2e.end()) && (at < (*er.first).second))
        {
            ret = extend(er.first, up);
        }
        else if ((er.second != b2e.end()) && (at == er.second->first))
        {
            ret = extend(er.second, up);
        }
        else // (er.first == er.second)
        {
            if (iter == b2e.begin())
            {
                valt_t &old = *iter;
                if (at + 1 < old.first)
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
                else
                {
                    b2e.insert(iter, valt_t(at, old.second));
                    b2e.erase(iter);
                }
            }
            else if (iter == b2e.end())
            {
                valt_t &old = *b2e.rbegin();
                if (old.second < at)
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
                else if (old.second == at)
                {
                    old.second = at + 1;
                }
            }
            else
            {
                valt_t &old = *iter;
                riter_t riter(iter);
                --riter;
                valt_t &rold = *riter;
                if (rold.second + 1 == old.first) // rold.second == at
                {
                    rold.second = old.second;
                    b2e.erase(iter);
                }
                else
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
            }
        }
    }
    return ret;
}

u_t Intervals::extend(iter_t iter, bool up)
{
    u_t ret = (up ? iter->second : iter->first - 1);
    if (up)
    {
        iter_t inext(iter);
        ++inext;
        if ((inext != b2e.end()) && (ret == inext->first))
        {
            (*iter).second = (*inext).second;
            b2e.erase(inext);
        }
    }
    else // down
    {
        const valt_t& old = *iter;
        if (iter == b2e.begin())
        {
            b2e.insert(iter, valt_t(ret, old.second));
        }
        else
        {
            riter_t riter(iter);
            ++riter;
            valt_t& rold = *riter;
            if (rold.second == ret)
            {
                rold.second = old.second;
            }
            else
            {
                b2e.insert(iter, valt_t(ret, old.second));
            }
        }
        b2e.erase(iter);
    }
    return ret;
}

#include <vector>
typedef vector<bool> vb_t;

class IntervalsNaive
{
 public:
    u_t add(u_t at, bool up)
    {
        u_t ret = at;
        if (at >= a.size())
        {
            a.insert(a.end(), at + 1 - a.size(), false);
        }
        if (a[at])
        {
            if (up)
            {
                while ((ret < a.size()) && a[ret])
                {
                    ++ret;
                }
                if (ret < a.size())
                {
                    a[ret] = true;
                }
                else
                {
                    a.push_back(true);
                }
            }
            else
            {
                for (; (ret > 0) && a[ret]; --ret) {}
                a[ret] = true;
            }
        }
        else
        {
            a[at] = true;
        }
        return ret;
    }
    u_t size() const 
    {
        u_t sz = 0;
        bool last = false;
        for (auto x : a)
        {
            if (x && !last)
            {
                ++sz;
            }
            last = x;
        }
        return sz;
    };
 private:
    vb_t a;
};

#include <cstdlib>
#include <string>

static int test_random(int argc, char **argv)
{
    typedef pair<u_t, bool> ub_t;
    typedef vector<ub_t> vub_t;
    int rc = 0;
    u_t ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_add = strtoul(argv[ai++], 0, 0);
    u_t b = strtoul(argv[ai++], 0, 0);
    u_t e = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (ti < n_tests) && (rc == 0); ++ti)
    {
        Intervals intervals;
        IntervalsNaive intervals_naive;
        vub_t input;
        
        for (u_t ia = 0; (ia < n_add) && (rc == 0); ++ia)
        {
            u_t at = b + rand() % (e - b);
            bool up = ((rand() % 2) == 0);
            input.push_back(ub_t(at, up));
            u_t rat = intervals.add(at, up);
            u_t nrat = intervals_naive.add(at, up);
            u_t sz = intervals.size();
            u_t nsz = intervals_naive.size();
            if ((rat != nrat) || (sz != nsz))
            {
                rc = 1;
                cerr << "rat="<<rat << ", nrat="<<nrat<<'\n';
                cerr << "Sizes: intervals=" << sz << " Naive="<<nsz << '\n';
                cerr << "... explicit";
                for (const ub_t nup: input)
                {
                    cerr << ' ' << (nup.second ? '+' : '-') << nup.first;
                }
                cerr << '\n';
            }
        }
    }
    return rc;
}

static int test_explicit(int argc, char ** argv)
{
    int rc = 0;
    Intervals intervals;
    for (int ai = 0; (rc == 0) && (ai < argc); ++ai)
    {
        const char *a = argv[ai];
        char sign = *a;
        if ((sign == '+') || (sign == '-'))
        {
            ++a;
            u_t at = strtoul(a, 0, 0);
            u_t rat = intervals.add(at, (sign == '+'));
            cout << "at="<<argv[ai] << ", rat="<<rat << '\n';
        }
        else
        {
            cerr << "bad sig=" << sign << '\n';
            rc = 1;
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    string cmd(argc > 1 ? argv[1] : "");
    if (cmd == "random")
    {
        rc = test_random(argc - 2, argv + 2);
    }
    else if (cmd == "explicit")
    {
        rc = test_explicit(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Bad command: " << cmd << '\n';
        rc = 1;
    }
    return rc;
}
