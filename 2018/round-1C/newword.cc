// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef set<string> sets_t;
typedef set<char> setc_t;
typedef vector<setc_t> vsetc_t;

static unsigned dbg_flags;

static const ul_t LMAX = 10;

class NewWord
{
 public:
    NewWord(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void naive_subsolve(const string &head);
    void subsolve(const string &head);
    bool found() const { return new_word.size() > 0; }
    u_t N, L;
    vs_t old_words;
    sets_t old_words_set;
    vsetc_t oldchars;
    string new_word;
};

NewWord::NewWord(istream& fi)
{
    fi >> N >> L;
    old_words.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        string s;
        fi >> s;
        old_words.push_back(s);
        old_words_set.insert(old_words_set.end(), s);
    }
}

void NewWord::solve_naive()
{
    naive_subsolve("");
}

void NewWord::naive_subsolve(const string &head)
{
    u_t sz = head.size();
    if (sz == L)
    {
        if (old_words_set.find(head) == old_words_set.end())
        {
            new_word = head;
        }
    }
    else
    {
        const char *chead = head.c_str();
        char bhead[11];
        strcpy(bhead, chead);
        bhead[sz] = '\0';
        bhead[sz + 1] = '\0';
        for (u_t i = 0; (i != N) && !found(); ++i)
        {
            bhead[sz] = old_words[i][sz];
            string newhead(bhead);
            naive_subsolve(newhead);
        }
    }
}

void NewWord::solve()
{
    ull_t possibles = 1;
    for (u_t li = 0; li < L; ++li)
    {
        setc_t pos_oldc;
        for (u_t ni = 0; ni < N; ++ni)
        {
            pos_oldc.insert(pos_oldc.end(), old_words[ni][li]);
        }
        oldchars.push_back(pos_oldc);
        ull_t sz = pos_oldc.size();
        possibles *= sz;
    }

    if (old_words_set.size() < possibles)
    {
        subsolve("");        
    }
}

void NewWord::subsolve(const string &head)
{
    u_t sz = head.size();
    if (sz == L)
    {
        if (old_words_set.find(head) == old_words_set.end())
        {
            new_word = head;
        }
    }
    else
    {
        const char *chead = head.c_str();
        char bhead[11];
        strcpy(bhead, chead);
        bhead[sz] = '\0';
        bhead[sz + 1] = '\0';
        const setc_t &ocs = oldchars[sz];
        for (setc_t::const_iterator ci = ocs.begin(), ce = ocs.end(); 
            (ci != ce) && !found(); ++ci)
        {
            char c = *ci;
            bhead[sz] = c;
            string newhead(bhead);
            subsolve(newhead);
        }
    }
}

void NewWord::print_solution(ostream &fo) const
{
    if (found())
    {
        fo << ' ' << new_word;
    }
    else
    {
        fo << " -";
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

    void (NewWord::*psolve)() =
        (naive ? &NewWord::solve_naive : &NewWord::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        NewWord newword(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (newword.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        newword.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
