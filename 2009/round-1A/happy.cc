// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef map<ul_t, ul_t> ul2ul_t;

static unsigned dbg_flags;

static ul2ul_t happy_data;

static void happy_data_load()
{
    ifstream f("happy.data");
    while (!f.eof())
    {
        string s_bases_bits, s_n;
        f >> s_bases_bits >> s_n;
        if (!f.eof())
        {
            ul_t bases_bits = strtoul(s_bases_bits.c_str(), 0, 16);
            ul_t n = strtoul(s_n.c_str(), 0, 0);
            if (happy_data.size() < 6 || n < 10) 
            { 
                cerr << "s_bases_bits="<<s_bases_bits << ", s_n="<<s_n << 
                    ", bases_bits=0x" << hex << bases_bits <<
                    ", n=" << dec << n << "\n";
            }
            ul2ul_t::value_type v(bases_bits, n);
            happy_data.insert(happy_data.end(), v);
        }
    }
    cerr << "happy(0x7fc)=" << happy_data[0x7fc] << "\n";
}

class Happy
{
 public:
    Happy(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    vul_t bases;
    ul_t solution;
};

Happy::Happy(istream& fi) : solution(0)
{
    string line;
    getline(fi, line);
    const char *cline = line.c_str();
    bool more = true;
    while (more)
    {
        char *end;
        unsigned base = strtoul(cline, &end, 10);
        if ((more = (end != cline)))
        {
            bases.push_back(base);
            cline = end;
        }
    }
}

void Happy::solve()
{
    ul_t base_bits = 0;
    for (auto i = bases.begin(), e = bases.end(); i != e; ++i)
    {
        ul_t b = *i;
        ul_t bb = (1ul << b);
        base_bits |= bb;
    }
    auto w = happy_data.find(base_bits);
    if (w == happy_data.end())
    {
        cerr << "happy_data missing: " << hex << base_bits << dec << "\n";
        exit(1);
    }
    solution = (*w).second;
}

void Happy::print_solution(ostream &fo) const
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

    happy_data_load();

    unsigned n_cases;
    *pfi >> n_cases;
    string dummy_line;
    getline(*pfi, dummy_line);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Happy problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
