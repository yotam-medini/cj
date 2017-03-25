// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
// #include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef vector<unsigned> vu_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

enum { PLUS = -2, MINUS = -1};

enum QDir { Q_SELF, Q_E, Q_N, Q_W, Q_S };

// typedef map<int, QDir> n2q_t;
typedef map<int, string> n2s_t;

class Cell
{
 public:
    Cell(int vv=0): v(vv)
    {
        if (v >= 0)
        {
            const string s(size_t(1), c());
            found.insert(found.end(), n2s_t::value_type(v, s));
        }
    }
    int v; // PLUS, MINUS or digit
    n2s_t found;
    char c() const { return v2c(v); }
    string str() const;
    static char v2c(int v)
    {
        char ret = (v >= 0
           ? char('0' + v)
           : (v == PLUS ? '+' : '-'));
        return ret;
    }
};
typedef vector<Cell> vcell_t;

string Cell::str() const
{
    ostringstream os;
    os << "["<< v2c(v) << "{";
    const char *sep = "";
    for (auto &kv: found)
    {
        const string& s = kv.second;
        os << sep << kv.first << ":" << s;
        sep = ", ";
    }
    os << "}]";
    return os.str();
}

class SquareMath
{
 public:
    SquareMath(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned ij2i(unsigned i, unsigned j) const { return w*i + j; }
    string find_solution(int n) const;
    void iterate();
    void iterate_mode(bool di);
    static void ijq(unsigned &i, unsigned &j, unsigned quad);
    bool ijq_safe(unsigned &i, unsigned &j, unsigned quad);
    bool better(const string &curr, const string &candidate);
    void improve(string &curr, const string &candidate)
    {
        if (better(curr, candidate))
        {
            curr = candidate;
        }
    }
    void bug() const
    {
        cerr << "BUG reacjed!\n";
        debug_print();
        cerr << "Please GDB me...\n";
        exit(1);
    }
    void debug_print() const;
    unsigned w;
    vu_t q;
    vcell_t cells;
    vs_t solution;
};

void SquareMath::ijq(unsigned &i, unsigned &j, unsigned quad){
    switch (quad)
    {
     case Q_E: ++i; break;
     case Q_N: ++j; break;
     case Q_W: --i; break;
     case Q_S: --j; break;
     default:;    
    }
}

bool SquareMath::ijq_safe(unsigned &i, unsigned &j, unsigned quad){
    bool ok = true;
    unsigned next;
    switch (quad)
    {
     case Q_E: 
         ok = ((next = i + 1) < w);
         if (ok) { i = next; }
         break;
     case Q_N:
         ok = ((next = j + 1) < w);
         if (ok) { j = next; }
         break;
     case Q_W:
         ok = i > 0;
         if (ok) { --i; }
         break;
     case Q_S:
         ok = j > 0;
         if (ok) { --j; }
         break;
     default:;    
    }
    return ok;
}

SquareMath::SquareMath(istream& fi)
{
    unsigned nq;
    fi >> w >> nq;
    cells = vcell_t(vcell_t::size_type(w * w), Cell());
    for (int j = w - 1; j >= 0; --j)
    {
        string s;
        fi >> s;
        for (unsigned i = 0; i < w; ++i)
        {
            const char c = s[i];
            int v = (c == '+' 
                ? PLUS
                : (c == '-'
                    ? MINUS
                    : c - '0'));
            cells[ij2i(i, j)] = Cell(v);
        }
    }
    q = vu_t(vu_t::size_type(nq), 0);
    for (unsigned qi = 0; qi < nq; ++qi)
    {
        fi >> q[qi];
    }
    solution = vs_t(vs_t::size_type(nq), string());
    if (dbg_flags & 0x1) { debug_print(); }
}

void SquareMath::solve_naive()
{
}

void SquareMath::solve()
{
    static const string sempty("");
    bool any_empty = true;
    unsigned iter = 0;
    while (any_empty && (iter < 1000))
    {
        any_empty = false;
        for (unsigned qi = 0; qi < q.size(); ++qi)
        {
            if (solution[qi] == sempty)
            {
                solution[qi] = find_solution(q[qi]);
                any_empty = any_empty || (solution[qi] == sempty);
            }
        }
        if (any_empty)
        {
            iterate();
        }
    }
    
}

string SquareMath::find_solution(int n) const
{
    static const string sempty("");
    string ret = "";
    for (unsigned i = 0; i < w; ++i)
    {
        for (unsigned j = 0; j < w; ++j)
        {
            const Cell& cell = cells[ij2i(i, j)];
            if (cell.v >= 0)
            {
                auto w = cell.found.find(n);
                if (w != cell.found.end())
                {
                    if (dbg_flags & 0x4) { debug_print(); }
                    string candid(w->second);
                    if ((ret == sempty) || (ret > candid))
                    {
                        ret = candid;
                    }
                }
            }
        }
    }
    return ret;
}

void SquareMath::iterate()
{
    if (dbg_flags & 0x2) { debug_print(); }
    iterate_mode(false);
    if (dbg_flags & 0x2) { debug_print(); }
    iterate_mode(true);
    if (dbg_flags & 0x2) { debug_print(); }
}

void SquareMath::iterate_mode(bool digit)
{
    for (unsigned i = 0; i < w; ++i)
    {
        for (unsigned j = 0; j < w; ++j)
        {
            Cell& cell = cells[ij2i(i, j)];
            if (digit == (cell.v >= 0))
            {
                const string s0(size_t(1), cell.c());
                for (unsigned q = Q_E; q <= Q_S; ++q)
                {
                    unsigned i1 = i, j1 = j;
                    if (ijq_safe(i1, j1, q))
                    {
                        const Cell& nextcell = cells[ij2i(i1, j1)];
                        for (auto &kv : nextcell.found)
                        {
                            int n = (cell.v >= 0
                                ? cell.v + kv.first
                                : (cell.v == PLUS
                                    ? kv.first
                                    : (kv.first - 2*nextcell.v /* MINUS */)));
                            const string candidate = s0 + kv.second;
                            auto oldw = cell.found.find(n);
                            const n2s_t::value_type vc(n, candidate);
                            if (oldw == cell.found.end())
                            {
if (dbg_flags & 0x8) { cerr << "ij=("<<i<<","<<j<<"): n="<<n << ",q="<<
 ".>^<v"[q] << " " << candidate << "\n"; }
                                cell.found.insert(cell.found.end(), vc);
                            }
                            else if (better(oldw->second, candidate))
                            {
if (dbg_flags & 0x8) { cerr << "[U] ij=("<<i<<","<<j<<"): n="<<n << ",q="<<
 ".>^<v"[q] << " " << candidate << "\n"; }
                                oldw->second = candidate;
                            }
                        }
                    }
                }
            }
        }
    }
}

bool SquareMath::better(const string &curr, const string &candidate)
{
    bool ret = curr.empty();
    if (!ret)
    {
        unsigned curr_len = curr.length();
        unsigned candidate_len = curr.length();
        ret = (curr_len > candidate_len) ||
            ((candidate_len == candidate_len) && (curr > candidate));
    }
    return ret;
}

void SquareMath::print_solution(ostream &fo) const
{
    for (auto &s : solution)
    {
        fo << "\n" << s;
    }
}

void SquareMath::debug_print() const
{
    int maxlen  = 0;
    for (bool wet : {false, true})
    {
        for (int j = w - 1; j >= 0; --j)
        {
            for (unsigned i = 0; i < w; ++i)
            {
                string s = cells[ij2i(i, j)].str();
                int slen = s.length();
                if (wet)
                {
                    string spaces(size_t(maxlen - slen + 2), ' ');
                    cerr << s << spaces;
                }
                else
                {
                    if (maxlen < slen) { maxlen = slen; }
                }
            }
            cerr << "\n";
        }
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (SquareMath::*psolve)() =
        (naive ? &SquareMath::solve_naive : &SquareMath::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        SquareMath problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
