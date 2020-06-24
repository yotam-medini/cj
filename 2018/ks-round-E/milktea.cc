// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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
typedef vector<u_t> vu_t;
typedef set<vb_t> setvb_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;

static unsigned dbg_flags;

typedef pair<u_t, vu_t> u_vu_t;
typedef vector<u_vu_t> v_u_vu_t;
typedef set<u_vu_t> set_u_vu_t;
// assuming n < 2^inc.size()
void subsums(v_u_vu_t& sum_idx, const vu_t& inc, u_t n)
{
    
    sum_idx.clear();
    sum_idx.reserve(n);
    set_u_vu_t heap;
    heap.insert(heap.end(), u_vu_t(inc[0], vu_t(size_t(1), 0)));
    while (sum_idx.size() < n)
    {
        sum_idx.push_back(*heap.begin());
        const u_vu_t& curr = sum_idx.back();
        heap.erase(heap.begin());
        const u_t curr_sum = curr.first;
        const vu_t curr_idx = curr.second;
        if (curr_idx[0] != 0)
        {
            vu_t idx;
            idx.push_back(0);
            idx.insert(idx.end(), curr_idx.begin(), curr_idx.end());
            u_vu_t v(curr_sum + inc[0], idx);
            if ((inc[0] > 0) ||
                !binary_search(sum_idx.begin(), sum_idx.end(), v))
            {
                heap.insert(heap.end(), v);
            }
        }
        for (u_t i = 0; i < curr_idx.size(); ++i)
        {
            u_t ii = curr_idx[i];
            if ((ii + 1 < inc.size()) &&
                ((i + 1 == curr_idx.size()) || (ii + 1 < curr_idx[i + 1])))
            {
                vu_t idx(curr_idx);
                idx[i] = ii + 1;
                u_t delta = inc[ii + 1] - inc[ii];
                u_t ss = curr_sum + delta;
                u_vu_t v(ss, idx);
                if ((delta > 0) ||
                    !binary_search(sum_idx.begin(), sum_idx.end(), v))
                {
                    heap.insert(heap.end(), u_vu_t(ss, idx));
                }
            }
        }
    }
}

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
    void init_base_complaint();
    u_t N, M, P;
    vs_t friends;
    vs_t forbid;
    vvb_t bfriends;
    setvb_t bforbid;
    u_t solution;
    vb_t base;
    vuu_t complaint_index;
};

MilkTea::MilkTea(istream& fi) : solution(0)
{
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
    init();
    init_base_complaint();
    if (bforbid.find(base) != bforbid.end())
    {
        vu_t complaints;
        const u_t nc = complaint_index.size(); // <= P
        complaints.reserve(nc);
        for (u_t i = 0; i < nc; ++i)
        {
            complaints.push_back(complaint_index[i].first);
        }
        v_u_vu_t sub_complaints;
        subsums(sub_complaints, complaints, M);
        bool found = false;
        for (u_t i = 0; !found; ++i)
        {
            vb_t candidate(base);
            const u_vu_t& subc = sub_complaints[i];
            for (u_t idx: subc.second)
            {
                u_t pi = complaint_index[idx].second;
                candidate[pi] = !candidate[pi];
            }
            found = bforbid.find(candidate) == bforbid.end();
            if (found)
            {
                solution += subc.first;
            }
        }
    }
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

void MilkTea::init_base_complaint()
{
    base.reserve(P);
    complaint_index.reserve(P);
    for (u_t i = 0; i < P; ++i)
    {
        u_t count[2] = {0, 0};
        for (u_t fi = 0; fi < N; ++fi)
        {
            u_t zo = int(bfriends[fi][i]);
            ++count[zo];
        }
        bool base_value = count[0] < count[1];
        base.push_back(base_value);
        u_t minval = min(count[0], count[1]);
        u_t maxval = N - minval;
        solution += minval;
        u_t complaint_value = maxval - minval;
        // we could consider treating complaint_value=0 indexes separately
        uu_t compl_idx(complaint_value, i);
        complaint_index.push_back(compl_idx);
    }
    sort(complaint_index.begin(), complaint_index.end());
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
