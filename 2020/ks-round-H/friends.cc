// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <array>
// #include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<string> vs_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Friends
{
 public:
    Friends(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    int dist(const string& s0, const string& s1) const;
    bool are_friends(const string& s0, const string& s1) const;
    u_t N, Q;
    vs_t names;
    vau2_t qfs;
    vi_t solution;
};

Friends::Friends(istream& fi)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> N >> Q;
    names.reserve(N);
    copy_n(istream_iterator<string>(fi), N, back_inserter(names));
    qfs.reserve(Q);
    for (u_t i = 0; i < Q; ++i)
    {
        u_t i0, i1;
        fi >> i0 >> i1;
        au2_t qf{i0 - 1, i1 - 1};
        qfs.push_back(qf);
    }
}

void Friends::solve_naive()
{
    solution.reserve(Q);
    for (const au2_t& qf: qfs)
    {
        int d = dist(names[qf[0]], names[qf[1]]);
        solution.push_back(d == -1 ? -1 : d + 1);
    }
}

void Friends::solve()
{
    solve_naive();
}

int Friends::dist(const string& s0, const string& s1) const
{
    typedef pair<u_t, string> us_t;
    typedef set<us_t> queue_t;
    int d = -1;
    queue_t queue;
    set<string> black;
    queue.insert(us_t{0, s0});
    black.insert(black.end(), s0);
    while ((d == -1) && !queue.empty())
    {
        const us_t ds = *queue.begin();
        const u_t s0d = ds.first;
        const string node = ds.second;
        queue.erase(queue.begin());
        for (const string& a: names)
        {
            if (black.find(a) == black.end())
            {
                if (are_friends(node, a))
                {
                    if (a == s1)
                    {
                        d = s0d + 1;
                    }
                    black.insert(black.end(), a);
                    queue.insert(us_t{s0d + 1, a});
                }
            }
        }
    }
    return d;
}

bool Friends::are_friends(const string& s0, const string& s1) const
{
    bool are = false;
    for (size_t i = 0, e = s0.size(); (i != e) && !are; ++i)
    {
        are = s1.find(s0[i]) != string::npos;
    }
    return are;
}

void Friends::print_solution(ostream &fo) const
{
    for (int d: solution)
    {
        fo << ' ' << d;
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Friends::*psolve)() =
        (naive ? &Friends::solve_naive : &Friends::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Friends friends(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (friends.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        friends.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
