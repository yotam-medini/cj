// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef vector<int> vbool_t;

static unsigned dbg_flags;

enum { S_MAX = 100 };

string vb2s(const vbool_t& vb)
{
    char *cs = new char[vb.size() + 1];
    for (unsigned i = 0; i < vb.size(); ++i)
    {
        cs[i] = (vb[i] ? '+' : '-');
    }
    cs[vb.size()] = '\0';
    string ret = string(cs);
    delete [] cs;
    return ret;
}

class Pancakes
{
 public:
    Pancakes(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    string vb2s() const { return ::vb2s(faces); }
    vbool_t faces;
    unsigned n_pancakes;
    // int find_last_blank() const;
    unsigned solution;
// vbool2u_t memo;
};

Pancakes::Pancakes(istream& fi) : solution(0)
{
    string s;
    fi >> s;
    const char *cs = s.c_str();
    n_pancakes = strlen(cs);
    for (unsigned i = 0; i < n_pancakes; ++i)
    {
        faces.push_back(cs[i] == '+');
    }
}

void Pancakes::solve()
{
    auto faces_begin = faces.begin();
    auto faces_rbegin = faces.rbegin();
    if (dbg_flags & 0x1) { cerr << "init: " << vb2s() << "\n"; }
    auto rlast_false = find(faces_rbegin, faces.rend(), false);
    while (rlast_false != faces.rend())
    {
        vbool_t::iterator last_false(rlast_false.base());
        if (dbg_flags & 0x2) {
            cerr << "last_false=" << (last_false - faces_begin) << "\n"; }
        // We need a False beginning
        if (faces.front())
        {
            // We want to flip initial TRUEs
            auto first_false = find(faces_begin, last_false, false);
            if (first_false != last_false)
            {
                // To reverse [begin, first_true+1) we merely fill with False.
                fill(faces_begin, first_false, false);
                ++solution;
                if (dbg_flags & 0x1) { cerr << "[0]=True: " << vb2s() << "\n"; }
            }
        }
        // Now [front]=[0] == False
        reverse(faces_begin, last_false);
        for (auto i = faces_begin; i != last_false; ++i) { *i = !(*i); }
        ++solution;
        if (dbg_flags & 0x1) { cerr << "Progress: " << vb2s() << "\n"; }
        rlast_false = find(rlast_false, faces.rend(), false);
    }
}

void Pancakes::print_solution(ostream &fo)
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pancakes pancakes(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        pancakes.solve();
        fout << "Case #"<< ci+1 << ":";
        pancakes.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

