// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
// #include <sstream> // for debug
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

typedef map<char, u_t> c2u_t;
typedef map<string, u_t> s2u_t;
typedef set<string> sets_t;
typedef set<u_t> setu_t;
// typedef map<u_t, sets_t> u2sets_t;
typedef vector<bool> vb_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef map<u_t, vb_t> u2vb_t;

enum {WMAX = 10};

static unsigned dbg_flags;

class WordSet
{
 public:
    WordSet(const setu_t &vw=setu_t(), u_t vaz=0) : wis(vw), azmask(vaz) {}
    setu_t wis;
    u_t azmask;
};
typedef map<u_t, WordSet> u2ws_t;

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
    u_t word_get_cmasks(const string &s, c2u_t &cmask, u_t &azmask) const;
    void solve_letters(const string &letters);
    void dict_best_word(const vs_t d, const string &letters, string &best);
    u_t cost_lw(const string &letters, u_t wi) const;
    u_t wis2azmask(const setu_t &wis) const;
    void cleft_decrease(u_t *cleft, u_t sz, u_t di) const;
    u_t n, m;
    vs_t dict;
    vs_t lletters;
    // s2u_t word2idx;
    WordSet ws_sz[WMAX + 1];
    // setu_t csz_words[26][WMAX + 1];
    vb_t csz_words[26][WMAX + 1];
    // u2ws_t cszpat_words[26][WMAX + 1];
    u_t ccount[WMAX + 1][26];
    vu_t dicts[WMAX + 1];
    // vb_t cszpat2wi[26][WMAX + 1][1u << WMAX];
    u2vb_t cszpat2wi[26][WMAX + 1];
    vs_t solution;
};

#if 0
static string uhex(u_t n) {
  stringstream ss;
  ss << "0x" << hex << n;
  return ss.str();
}
static void setu_show(const string &msg, const setu_t& su) {
    cerr << msg << ' ';
    copy(su.begin(), su.end(), ostream_iterator<u_t>(cerr, " "));
    cerr << '\n';
}
#endif

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
    c2u_t cmask;
    for (u_t sz = 0; sz <= WMAX; ++sz)
    {
        fill_n(&ccount[sz][0], 26, 0);
    }
    for (u_t i = 0; i < n; ++i)
    {
        const string &word = dict[i];
        // word2idx.insert(word2idx.end(), s2u_t::value_type(word, i));
        u_t sz = word.size();
        dicts[sz].push_back(i);
        // ws_sz[sz].wis.insert(ws_sz[sz].wis.end(), i);
        
        for (char c: word)
        {
            u_t ci = c - 'a';
            // setu_t &wis = csz_words[ci][sz];
            // wis.insert(wis.end(), i);
            ++ccount[sz][ci];
        }
    }
    for (u_t sz = 1; sz <= WMAX; ++sz)
    {
        u_t dsz = dicts[sz].size();
        for (u_t ci = 0; ci < 26; ++ci)
        {
            csz_words[ci][sz] = vb_t(vb_t::size_type(dsz), false);
            // for (u_t pat = 0; pat < (1u << WMAX); ++pat)
            // { cszpat2wi[ci][sz][pat] = vb_t(vb_t::size_type(dsz), false); }
        }
        const vb_t vb0 = vb_t(vb_t::size_type(dsz), false);
        for (u_t di = 0; di < dsz; ++di)
        {
            u_t i = dicts[sz][di];
            const string &word = dict[i];
            // word2idx.insert(word2idx.end(), s2u_t::value_type(word, i));
            u_t azmask = 0;
            word_get_cmasks(word, cmask, azmask);
            for (auto const &x: cmask)
            {
                char c = x.first;
                u_t ci = c - 'a';
                // ws_sz[sz].azmask |= (1u << (c - 'a'));
                u_t mask = x.second;
                u2vb_t & csz2wi = cszpat2wi[ci][sz];
                u2vb_t::iterator pi = csz2wi.find(mask);
                if (pi == csz2wi.end())
                {
                    u2vb_t::value_type v(mask, vb0);
                    pi = csz2wi.insert(csz2wi.end(), v);
                }
                (*pi).second[di] = true;
                csz_words[ci][sz][di] = true;
            }
        }
    }
}

u_t KillWord::word_get_cmasks(const string &s, c2u_t &cmask, u_t &azmask) const
{
    u_t sz = s.size();
    cmask.clear();
    azmask = 0;
    for (u_t p = 0; p < sz; ++p)
    {
        char c = s.at(p);
        azmask |= (1u << (c - 'a'));
        c2u_t::iterator where = cmask.find(c);
        if (where == cmask.end())
        {
            cmask.insert(c2u_t::value_type(c, 1u << p));
        }
        else
        {
            where->second |= (1u << p);
        }
    }
    return sz;
}

void KillWord::solve_letters(const string &letters)
{
    string best_word;
    dict_best_word(dict, letters, best_word);
    solution.push_back(best_word);
}

void KillWord::dict_best_word(const vs_t d, const string &letters, string &best)
{
    u_t best_wi = 0;
    u_t max_cost = 0;
    for (u_t wi = 0; wi < n; ++wi)
    {
        u_t cost = cost_lw(letters, wi);
        if (max_cost < cost)
        {
            max_cost = cost;
            best_wi = wi;
        }
    }
    best = dict[best_wi];
}

void KillWord::cleft_decrease(u_t *cleft, u_t sz, u_t di) const
{
    u_t wi = dicts[sz][di];
    const string &word = dict[wi];
    for (char c: word)
    {
        u_t ci = c - 'a';
        --cleft[ci];
    }
}

u_t KillWord::cost_lw(const string &letters, u_t wi) const
{
    u_t cost = 0;
    string scost;
    const string &word = dict[wi];
    c2u_t word_cmask;
    u_t word_azmask;
    u_t sz = word_get_cmasks(word, word_cmask, word_azmask);
    u_t dsz = dicts[sz].size();
    setu_t ws = ws_sz[sz].wis;
    vb_t word_canceled = vb_t(vb_t::size_type(dsz), false);
    u_t cleft[26];
    copy(&ccount[sz][0], &ccount[sz][0]+26, &cleft[0]);
    const u_t all_mask = (1u << sz) - 1;
    u_t matched_mask = 0;
    for (u_t li = 0; (li < letters.size()) && (matched_mask != all_mask); ++li)
    {
        char c = letters[li];
        u_t ci = (c - 'a');
        if (cleft[ci] > 0)        
        {
            auto wci = word_cmask.find(c);
            if (wci != word_cmask.end())
            {
                u_t cmask = (*wci).second;
                matched_mask |= cmask;
                u2vb_t::const_iterator pi = cszpat2wi[ci][sz].find(cmask);
                if (pi == cszpat2wi[ci][sz].end()) {
                    cerr << __LINE__ << " software error\n";
                }
                const vb_t &pat2wi = (*pi).second;
                for (u_t di = 0; di < dsz; ++di)
                {
                    if (!(pat2wi[di] || word_canceled[di]))
                    {
                        word_canceled[di] = true;
                        cleft_decrease(cleft, sz, di);
                    }
                }
            }
            else
            {
                ++cost;
                scost.append(1, c);
                // const setu_t &del_words = csz_words[c - 'a'][sz];
                // sets_diff(ws_new, ws, del_words);
                const vb_t &contain_wi = csz_words[ci][sz];
                for (u_t di = 0; di < dsz; ++di)
                {
                    if (contain_wi[di] && !word_canceled[di])
                    {
                        word_canceled[di] = true;
                        cleft_decrease(cleft, sz, di);
                    }
                }
            }    
        }
    }
    if (dbg_flags & 0x1) { 
         cerr << "wi="<<wi << ", cost("<<word<<")="<<cost<<" ["<<scost<<"]\n"; }
    
    return cost;
}

#if 0
u_t KillWord::cost_lw(const string &letters, u_t wi) const
{
    u_t cost = 0;
    string scost;
    const string &word = dict[wi];
    c2u_t word_cmask;
    u_t word_azmask;
    u_t sz = word_get_cmasks(word, word_cmask, word_azmask);
    setu_t ws = ws_sz[sz].wis;
    u_t azmask = ws_sz[sz].azmask;
    for (u_t li = 0; li < letters.size(); ++li)
    {
        char c = letters[li];
        u_t ci = (c - 'a');
        u_t cbit = (1u << ci);
        if (cbit & azmask)
        {
            setu_t ws_new;
            if (cbit & word_azmask)
            {
                const u2ws_t &u2ws = cszpat_words[ci][sz];
                auto wci = word_cmask.find(c);
                if (wci == word_cmask.end()) {
                    cerr << __LINE__ << " software error\n";
                }
                u_t cmask = (*wci).second;
                auto wsi = u2ws.find(cmask);
                if (wsi == u2ws.end()) {
                    cerr << __LINE__ << " software error\n";
                }
                const WordSet &mws = (*wsi).second;
                set_intersection(ws.begin(), ws.end(), 
                    mws.wis.begin(), mws.wis.end(), 
                    inserter(ws_new, ws_new.end()));
            }
            else
            {
                ++cost;
                scost.append(1, c);
                const setu_t &del_words = csz_words[c - 'a'][sz];
#if 0
                set_difference(ws.begin(), ws.end(),
                    del_words.begin(), del_words.end(),
                    inserter(ws_new, ws_new.end()));
#endif
                sets_diff(ws_new, ws, del_words);
            }    
            swap(ws, ws_new);
            azmask = wis2azmask(ws);
        }
    }
    if (dbg_flags & 0x1) { 
         cerr << "wi="<<wi << ", cost("<<word<<")="<<cost<<" ["<<scost<<"]\n"; }
    
    return cost;
}
#endif

u_t KillWord::wis2azmask(const setu_t &wis) const
{
    u_t azmask = 0;
    for (auto wi: wis)
    {
        const string &word = dict[wi];
        for (char c: word)
        {
            azmask |= (1u << (c - 'a'));
        }
    }
    return azmask;
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
