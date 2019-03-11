// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
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
typedef vector<bool> vb_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef map<u_t, vb_t> u2vb_t;

typedef array<u_t, 26> u26t;
static const u26t zu26t{{0,}};

enum {WMAX = 10};

static unsigned dbg_flags;

class USet
{
 public:
    USet(u_t vcapacity=0) : _v(vb_t::size_type(vcapacity), false) {}
    u_t size() const { return _s.size(); }
    u_t capacity() const { return _v.size(); }
    bool empty() const { return size() == 0; }
    const setu_t& values() const { return _s; }
    void add(u_t x) { _v[x] = true; _s.insert(_s.end(), x); }
    bool has(u_t x) const { return _v[x]; }
    void intersect_by(vu_t &erased, const USet &other, bool complement)
    {
        erased.clear();
        for (setu_t::iterator i = _s.begin(), i1 = i; i != _s.end(); i = i1)
        {
            ++i1;
            u_t x = *i;
            if (other.has(x) == complement)
            {
                _v[x] = false;
                _s.erase(i);
                erased.push_back(x);
            }
        }
    }
    bool intersects_with(const USet &other) const
    {
        bool ret = false; 
        if (size() > other.size())
        {
            ret = other.intersects_with(*this);
        }
        else
        {
            for (setu_t::iterator i = _s.begin(); (i != _s.end()) && !ret; ++i)
            {
                u_t x = *i;
                ret = other.has(x);
            }
        }
        return ret;
    }
 private:
    vb_t _v;
    setu_t _s;
};
typedef map<u_t, USet> u2uset_t;


class CharSpan
{
  public:
    CharSpan(u_t vcapacity=0) : luis(vcapacity) {}
    void add(u_t posmask, u_t lui)
    {
        auto er = posmask_luis.equal_range(posmask);
        auto i = er.first;
        if (er.first == er.second)
        {
            i = posmask_luis.insert(er.first,
                u2uset_t::value_type(posmask, USet(luis.capacity())));
        }
        (*i).second.add(lui);
        luis.add(lui);
    }
    u2uset_t posmask_luis;
    USet luis;
};

class MonoSize
{
 public:
    MonoSize() {}
    u_t n() const { return dict_lut.size(); }
    void show() const {
        cerr << "{\n";
        for (u_t ci=0; ci < 26; ++ci) {
            const CharSpan &cs = cspan[ci];
            const setu_t &luis = cs.luis.values();
            if (luis.size() > 0) {
                cerr << "abcdefghijklmnopqrstuvwxyz"[ci] << ": luis[" << 
                    cs.luis.size() << "]={";
                for (u_t lui: luis) { cerr << ", "<<lui; }
                cerr << "}\n";
            }
        }
        cerr << "}\n";
    }
    vu_t dict_lut;
    // u2vb_t cmask2_lui[26];
    // vb_t c2lui[26];
    CharSpan cspan[26];
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
    u_t cost_lw(const string &letters, u_t sz, u_t lui) const;
    // void cwc_init(u_t sz, u26t &cwc, const USet&) const;
    // void cwc_luis_decrease(u_t sz, u26t &cwc, const vu_t &luis) const;
    void show_maps() const;

    u_t n, m;
    vs_t dict;
    vs_t lletters;
    // vector<u26t> dict_cmasks; 
    vwc_t word_chars;
    vs_t solution;
    MonoSize mono_size[WMAX + 1];
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
    
    // u26t cmask;
    WordChars wc;
    for (u_t wi = 0; wi < n; ++wi)
    {
        const string &word = dict[wi];
        u_t sz = word_get_cmasks(word, wc.posmask);
        wc.chars.clear();
        // dict_cmasks.push_back(cmask);
        MonoSize &ms = mono_size[sz];
        u_t lui = ms.dict_lut.size();
        ms.dict_lut.push_back(wi);
        for (u_t ci = 0; ci < 26; ++ci)
        {
            u_t mask = wc.posmask[ci];
            if (mask > 0)
            {
                wc.chars.push_back(ci);
                ms.cspan[ci].add(mask, lui);
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
    for (u_t sz = 0; sz <= WMAX; ++sz) {
        const MonoSize &ms = mono_size[sz];
        if (ms.n() > 0) { cerr << "sz="<<sz << ":\n"; ms.show();  }
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
    u_t best_wi = 0;
    u_t max_cost = 0;
    for (u_t sz = 1; sz <= WMAX; ++sz)
    {
        const MonoSize &ms = mono_size[sz];
        for (u_t lui = 0, luie = ms.n(); lui < luie; ++lui)
        {
            u_t wi = ms.dict_lut[lui];
            u_t cost = cost_lw(letters, sz, lui);
            if ((max_cost < cost) || ((max_cost == cost) && (best_wi > wi)))
            {
                max_cost = cost;
                best_wi = wi;
            }
        }
    }
    best = dict[best_wi];
}

u_t KillWord::cost_lw(const string &letters, u_t sz, u_t lui) const
{
    u_t cost = 0;
    const MonoSize &ms = mono_size[sz];
    u_t wi = ms.dict_lut[lui];
    string scost;
    const string &word = dict[wi];
    const u26t &word_cmask = word_chars[wi].posmask;
    const u_t all_mask = (1u << sz) - 1;
    u_t matched_mask = 0;
    USet curr(sz);
    for (u_t i = 0, e = ms.n(); i < e; ++i) { curr.add(i); }
    for (u_t li = 0; (li < 26) && (matched_mask != all_mask); ++li)
    {
        char c = letters[li];
        u_t ci = c - 'a';
        const CharSpan &cs = ms.cspan[ci];
        if (curr.intersects_with(cs.luis))
        { 
            vu_t luis_erased;
            u_t posmask = word_cmask[ci];
            if (posmask > 0)
            {
                auto i = cs.posmask_luis.find(posmask);
                curr.intersect_by(luis_erased, i->second, false);
                matched_mask |= posmask;
            }
            else
            {
                ++cost;
                curr.intersect_by(luis_erased, cs.luis, true); // by complement
            }
        }
    }
    if (dbg_flags & 0x1) {
        cerr<<"cost(@"<<letters<<", "<<word<<")="<<cost<<" ["<<scost<<"]\n"; }
    return cost;
}

#if 0
void KillWord::cwc_init(u_t sz, u26t &cwc, const USet &us)
    const
{
    const MonoSize &ms = mono_size[sz];
    const setu_t &luis = us.values();
    for (u_t lui: luis)
    {
        u_t wi = ms.dict_lut[lui];
cerr << __LINE__ << " lui="<<lui << ", wi="<<wi << "\n";
        const u26t &cmask = dict_cmasks[wi];
        for (u_t ci = 0; ci < 26; ++ci)
        {
            if (cmask[ci] > 0)
            {
                cwc[ci] += 1;
            }
        }     
    }
}

void KillWord::cwc_luis_decrease(u_t sz, u26t &cwc, const vu_t &luis) const
{
    const MonoSize &ms = mono_size[sz];
    for (u_t lui: luis)
    {
        u_t wi = ms.dict_lut[lui];
        const u26t &cmask = dict_cmasks[wi];
        for (u_t ci = 0; ci < 26; ++ci)
        {
            if (cmask[ci] > 0)
            {
                cwc[ci] -= 1;
            }
        }     
    }
}
#endif

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
