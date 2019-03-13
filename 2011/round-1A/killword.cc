// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <algorithm>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef map<u_t, vu_t> u2vu_t;

typedef array<u_t, 26> u26t;
static const u26t zu26t{{0,}};

enum {WMAX = 10};

static unsigned dbg_flags;

class MonoSize
{
 public:
    MonoSize() {}
    u_t n() const { return dict_lut.size(); }
    vu_t dict_lut;
};

class WordChars
{
 public:
    WordChars() {}
    u26t posmask;
    vu_t chars;
};
typedef vector<WordChars> vwc_t;

class KillWord
 {
 public:
    KillWord(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_naive_letters(const string &letters);
    void dict_naive_best_word(const vs_t d, const string &letters, 
        string &best);
    u_t naive_cost_lw(const string &letters, const string &word) const;
    bool c_in_dict(char c, const vs_t &d) const;
    void dict_delc(vs_t &rdict, char c) const;
    void dict_word_c_filter(vs_t &d, const string &word, char c) const;
    void init_maps();
    u_t word_get_cmasks(const string &s, u26t &cmask) const;
    void solve_letters(const string &letters);
    void dict_best_word(const string &letters, string &best);
    void compute_costs(u_t sz, const vu_t &luis, const string &letters, u_t li);
    void show_maps() const;

    u_t n, m;
    vs_t dict;
    vs_t lletters;
    vs_t solution;
    vwc_t word_chars;
    MonoSize mono_size[WMAX + 1];
    vu_t costs;
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
    for (auto letters: lletters)
    {
        solve_naive_letters(letters);
    }
}

void KillWord::solve_naive_letters(const string &letters)
{
    string best_word;
    dict_naive_best_word(dict, letters, best_word);
    solution.push_back(best_word);
}

void KillWord::dict_naive_best_word(const vs_t d, const string &letters,
    string &best)
{
    best = *d.begin();
    u_t max_cost = 0;
    for (auto word: d)
    {
        u_t cost = naive_cost_lw(letters, word);
        if (max_cost < cost)
        {
            max_cost = cost;
            best = word;
        }
    }
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
    init_maps();
    for (auto letters: lletters)
    {
        solve_letters(letters);
    }
}

void KillWord::init_maps()
{
    word_chars.reserve(n);
    u26t mono_size_sizes = zu26t;
    
    for (u_t wi = 0; wi < n; ++wi)
    {
        const string &word = dict[wi];
        u_t sz = word.size();
        ++mono_size_sizes[sz];
    }

    WordChars wc;
    for (u_t wi = 0; wi < n; ++wi)
    {
        const string &word = dict[wi];
        u_t sz = word_get_cmasks(word, wc.posmask);
        wc.chars.clear();
        MonoSize &ms = mono_size[sz];
        ms.dict_lut.push_back(wi);
        for (u_t ci = 0; ci < 26; ++ci)
        {
            u_t mask = wc.posmask[ci];
            if (mask > 0)
            {
                wc.chars.push_back(ci);
            }
        }
        word_chars.push_back(wc);
    }
    if (dbg_flags & 0x2) { show_maps(); }
}

void KillWord::show_maps() const
{
    for (u_t wi = 0; wi < n; ++wi) {
        const string &word = dict[wi];
        const WordChars &wc = word_chars[wi];
        cerr << "word: "<<word << ", mask={";
        for (u_t ci: wc.chars)
        {
            if (wc.posmask[ci] > 0) { cerr << "  " << 
                "abcdefghijklmnopqrstuvwxyz"[ci] << ":" << 
                hex << wc.posmask[ci]; }
        }
        cerr << dec;
    }
}

u_t KillWord::word_get_cmasks(const string &s, u26t &cmask) const
{
    u_t sz = s.size();
    cmask = zu26t;
    for (u_t p = 0; p < sz; ++p)
    {
        char c = s.at(p);
        u_t ci = c - 'a';
        cmask[ci] |= (1u << p);
    }
    return sz;
}

void KillWord::solve_letters(const string &letters)
{
    string best_word;
    dict_best_word(letters, best_word);
    solution.push_back(best_word);
}

void KillWord::dict_best_word(const string &letters, string &best)
{
    costs = vu_t(vu_t::size_type(n), 0);
    for (u_t sz = 1; sz <= WMAX; ++sz)
    {
        const MonoSize &ms = mono_size[sz];
        u_t ms_sz = ms.n();
        if (ms_sz > 0)
        {
            vu_t luis(vu_t::size_type(ms_sz), 0);
            for (u_t li = 0; li < ms_sz; ++li) { luis[li] = li; }
            compute_costs(sz, luis, letters, 0);
        }
    }
    u_t best_wi = 0;
    u_t max_cost = 0;
    for (u_t wi = 0; wi < n; ++wi)
    {
        const string word = dict[wi];
        u_t cost = costs[wi];
        if (dbg_flags & 0x1) {
          cerr<<"cost(@"<<letters<<", "<<word<<")="<<cost<<"\n"; }
        if (max_cost < cost)
        {
            max_cost = cost;
            best_wi = wi;
        }
    }
    best = dict[best_wi];
}

void KillWord::compute_costs(u_t sz, const vu_t &luis, const string &letters,
    u_t li)
{
    const MonoSize &ms = mono_size[sz];
    const char c = letters.at(li);
    const u_t ci = c - 'a';
    u2vu_t pm2luis;
    u2vu_t::iterator p2li0 = pm2luis.end();
    for (u_t lui: luis)
    {
        u_t wi = ms.dict_lut[lui];
        u_t posmask = word_chars[wi].posmask[ci];
        auto er = pm2luis.equal_range(posmask);
        u2vu_t::iterator p2li = er.first;
        if (er.first == er.second)
        {
            p2li = pm2luis.insert(p2li, u2vu_t::value_type(posmask, vu_t()));
            if (posmask == 0) { p2li0 = p2li; }
        }
        (*p2li).second.push_back(lui);
    }
    if (p2li0 != pm2luis.end())
    {
        const vu_t &luis0 = (*p2li0).second;
        if (luis0.size() < luis.size())
        {
            for (u_t lui: luis0)
            {
                u_t wi = ms.dict_lut[lui];
                ++costs[wi];
            }
        }
    }
    if (++li < 26)
    {
        for (u2vu_t::const_iterator i = pm2luis.begin(), e = pm2luis.end();
            i != e; ++i)
        {
            compute_costs(sz, (*i).second, letters, li);
        }
    }
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
