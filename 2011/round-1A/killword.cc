// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef vector<string> vs_t;
// typedef set<string> sets_t;
// typedef set<char> setc_t;

static unsigned dbg_flags;

class KillWord
{
 public:
    KillWord(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive_letters(const string &letters);
    u_t naive_cost_lw(const string &letters, const string &word) const;
    bool c_in_dict(char c, const vs_t &d) const;
    void dict_delc(vs_t &rdict, char c) const;
    void dict_word_c_filter(vs_t &d, const string &word, char c) const;
    u_t n, m;
    vs_t dict;
    vs_t lletters;
    vs_t solution;
};

KillWord::KillWord(istream& fi)
{
   fi >> n >> m;
   dict.reserve(n);
   lletters.reserve(m);
   string s;
   for (u_t i = 0; i < n; ++i)
   {
       fi >> s;
       dict.push_back(s);
   }
   for (u_t i = 0; i < m; ++i)
   {
       fi >> s;
       lletters.push_back(s);
   }
}

void KillWord::solve_naive()
{
    // sort(dict.begin(), dict.end());
    for (auto letters: lletters)
    {
        solve_naive_letters(letters);
    }
}

void KillWord::solve_naive_letters(const string &letters)
{
    string best_word = *dict.begin();
    u_t max_cost = 0;
    for (auto word: dict)
    {
        u_t cost = naive_cost_lw(letters, word);
        if (max_cost < cost)
        {
            max_cost = cost;
            best_word = word;
        }
    }
    solution.push_back(best_word);
}

u_t KillWord::naive_cost_lw(const string &letters, const string &word) const
{
    u_t cost = 0;
    string scost;
    vs_t rdict = dict;
    u_t li = 0;
    size_t sz = word.size();
    string guess = string(sz, '_');
    dict_word_c_filter(rdict, word, ' '); // just the size
    while (guess != word)
    {
        if (li > letters.size()) { 
            cerr << "software error\n"; }
        while (!c_in_dict(letters[li], rdict))
        {
            ++li;
        }
        char c = letters[li++];
        if (word.find(c) == string::npos)
        {
            ++cost;
            dict_delc(rdict, c);
            scost.append(1, c);
        }
        else
        {
            for (u_t i = 0; i < sz; ++i)
            {
                if (word.at(i) == c)
                {
                    guess.at(i) = c;
                }
            }
            dict_word_c_filter(rdict, word, c);
        }
    }
    if (dbg_flags & 0x1) {
         cerr<<"cost(@"<<letters<<", "<<word<<")="<<cost<<" ["<<scost<<"]\n"; }
    return cost;
}

bool KillWord::c_in_dict(char c, const vs_t &d) const
{
    bool in_dict = false;
    for (vs_t::const_iterator i = d.begin(), e = d.end();
        (i != e) && !in_dict; ++i)
    {
        const string &w = *i;
        in_dict = (w.find(c) != string::npos);
    }
    return in_dict;
}

void KillWord::dict_delc(vs_t &d, char c) const
{
    vs_t ndict;
    for (vs_t::const_iterator i = d.begin(), e = d.end(); i != e; ++i)
    {
        const string &w = *i;
        bool hasc = (w.find(c) != string::npos);
        if (!hasc)
        {
            ndict.push_back(w);
        }
    }
    swap(ndict, d);
}

void KillWord::dict_word_c_filter(vs_t &d, const string &word, char c) const
{
    vs_t ndict;
    for (vs_t::const_iterator i = d.begin(), e = d.end(); i != e; ++i)
    {
        const string &w = *i;
        bool match = (w.size() == word.size());
        for (u_t ci = 0; match && (ci < w.size()); ++ci)
        {
            match = (w.at(ci) == c) == (word.at(ci) == c);
        }
        if (match)
        {
            ndict.push_back(w);
        }
    }
    swap(ndict, d);
}

void KillWord::solve()
{
    solve_naive();
}

void KillWord::print_solution(ostream &fo) const
{
    for (auto s : solution)
    {
        fo << ' ' << s;
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

    void (KillWord::*psolve)() =
        (naive ? &KillWord::solve_naive : &KillWord::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        KillWord killWord(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (killWord.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        killWord.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
