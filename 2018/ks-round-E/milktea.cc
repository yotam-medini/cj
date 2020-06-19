// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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
typedef vector<bool> vb_t;
typedef vector<string> vs_t;
typedef vector<vb_t> vvb_t;
typedef set<vb_t> setvb_t;

static unsigned dbg_flags;

class MilkTea
{
 public:
    MilkTea(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void init();
    void scan_offers(vb_t& offer);
    u_t n_complains(const vb_t& offer) const;
    u_t N, M, P;
    vs_t friends;
    vs_t forbid;
    vvb_t bfriends;
    setvb_t bforbid;
    u_t solution;
};

MilkTea::MilkTea(istream& fi) : solution(0)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> N >> M >> P;
    friends.reserve(N);
    forbid.reserve(M);
    copy_n(istream_iterator<string>(fi), N, back_inserter(friends));
    copy_n(istream_iterator<string>(fi), M, back_inserter(forbid));
}

void MilkTea::solve_naive()
{
    init();
    solution = u_t(-1); // infinite
    vb_t offer;
    offer.reserve(P);
    scan_offers(offer);
}

void MilkTea::scan_offers(vb_t& offer)
{
    if (offer.size() == P)
    {
        if (bforbid.find(offer) == bforbid.end())
        {
            u_t nc = n_complains(offer);
            if (solution > nc)
            {
                solution = nc;
            }
        }
    }
    else
    {
        offer.push_back(false);
        scan_offers(offer);
        offer.back() = true;
        scan_offers(offer);
        offer.pop_back();
    }
}

u_t MilkTea::n_complains(const vb_t& offer) const
{
    u_t nc = 0;
    for (const vb_t& bf: bfriends)
    {
        for (u_t i = 0; i < P; ++i)
        {
            nc += (bf[i] == offer[i] ? 0 : 1);
        }
    }
    return nc;
}

void MilkTea::solve()
{
    solve_naive();
}

void MilkTea::init()
{
    bfriends.reserve(N);
    for (const string& f: friends)
    {
        vb_t b;
        b.reserve(P);
        for (char c: f)
        {
            b.push_back(c == '1');
        }
        bfriends.push_back(b);
    }
    for (const string& fb: forbid)
    {
        vb_t b;
        b.reserve(P);
        for (char c: fb)
        {
            b.push_back(c == '1');
        }
        bforbid.insert(bforbid.end(), b);
    }
}

void MilkTea::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (MilkTea::*psolve)() =
        (naive ? &MilkTea::solve_naive : &MilkTea::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        MilkTea milktea(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (milktea.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        milktea.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
