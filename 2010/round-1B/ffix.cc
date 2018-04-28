// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef map<string, string> s2s_t;
typedef map<string, void *> s2p_t;

class Dir
{
 public:
    Dir() {}
    ~Dir()
    {
        for (auto i: s2p)
        {
            Dir *d = static_cast<Dir*>(i.second);
            delete d;
        }
    }
    Dir *find(const string &sub)
    {
        s2p_t::iterator w = s2p.find(sub);
        Dir *ret = static_cast<Dir*>(w != s2p.end() ? w->second : 0);
        return ret;
    }
    Dir *add(const string &sub)
    {
        Dir *d = new Dir;
        s2p_t::value_type v(sub, d);
        s2p.insert(s2p.end(), v);
        return d;
    }
    s2p_t s2p;
};

static unsigned dbg_flags;

class FFix
{
 public:
    FFix(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void parse(vs_t &path, const string &s);
    ull_t add_path(const vs_t &path);
    ul_t N, M;
    vs_t dirs_old;
    vs_t dirs_new;
    ull_t solution;
    Dir root;
};

FFix::FFix(istream& fi) : solution(0)
{
    fi >> N >> M;
    string s;
    dirs_old = vs_t(N, string());
    dirs_new = vs_t(M, string());
    for (u_t i = 0; i != N; ++i)
    {
        fi >> dirs_old[i];
    }
    for (u_t i = 0; i != M; ++i)
    {
        fi >> dirs_new[i];
    }
}

void FFix::solve_naive()
{
    vs_t path;
    for (const string &s: dirs_old)
    {
        parse(path, s);
        add_path(path);
    }
    solution = 0;
    for (const string &s: dirs_new)
    {
        parse(path, s);
        ull_t na = add_path(path);
        solution += na;
    }
}

void FFix::solve()
{
    solve_naive();
}

void FFix::parse(vs_t &path, const string &s)
{
    path.clear();
    const char *cs = s.c_str();
    u_t e;
    for (u_t b = 0; cs[b]; b = e)
    {
        ++b;
        for (e = b; cs[e] && cs[e] != '/'; ++e) {}
        string sub(cs + b, cs + e);
        path.push_back(sub);
    }
}

ull_t FFix::add_path(const vs_t &path)
{
    ull_t ret = 0;
    // advance while exist;
    Dir *d = &root, *nd;
    u_t di = 0, depth = path.size();
    while ((di < depth) && (nd = d->find(path[di])))
    {
        ++di;
        d = nd;
    }
    ret = depth - di;
    for (; di < depth; ++di)
    {
        d = d->add(path[di]);
    }
    return ret;
}

void FFix::print_solution(ostream &fo) const
{
    fo << " " << solution;
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

    void (FFix::*psolve)() =
        (naive ? &FFix::solve_naive : &FFix::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        FFix ffix(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ffix.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ffix.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
