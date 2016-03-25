// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

static unsigned dbg_flags;

typedef vector<char> vc_t;
typedef set<unsigned> setu_t;

static int QWERASDF_to_i(char c)
{
    static const char *QWERASDF = "QWERASDF";
    const char* p = strchr(QWERASDF, c);
    int ret = (p ? p - QWERASDF : -1);
    return ret;
}

class Magicka
{
 public:
    Magicka(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    void comb_set(char c0, char c1, char cnew)
    {
        int x = QWERASDF_to_i(c0);
        int y = QWERASDF_to_i(c1);
        comb[x][y] = comb[y][x] = cnew;
    }
    char comb_get(char c0, char c1) const
    {
        int x = QWERASDF_to_i(c0);
        int y = QWERASDF_to_i(c1);
        char ret = ((x != -1) && (y != -1) ? comb[x][y] : ' ');
        return ret;
    }
    void cancel_set(char c0, char c1)
    {
        int x = QWERASDF_to_i(c0);
        int y = QWERASDF_to_i(c1);
        cancel[x][y] = true;
        cancel[y][x] = true;
    }
    bool cancel_get(char c0, char c1) const
    {
        int x = QWERASDF_to_i(c0);
        int y = QWERASDF_to_i(c1);
        bool ret = ((x != -1) && (y != -1) ?  cancel[x][y] : false);
        return ret;
    }
#if 0
    void cancel_set(char c0, char c1)
    {
        cancel[c0 - 'A'][c1 - 'A'] = true;
        cancel[c1 - 'A'][c0 - 'A'] = true;
    }
    bool cancel_get(char c0, char c1) const
    {
        bool ret = cancel[c0 - 'A'][c1 - 'A'];
        return ret;
    }
#endif
    vc_t cseq_out;
    char comb[8][8];
    bool cancel[8][8];
    // bool cancel[26][26];
    string cseq_in;
    unsigned N;
};

Magicka::Magicka(istream& fi)
{
    for (unsigned x = 0; x < 8; ++x)
    {
        for (unsigned y = 0; y < 8; ++y)
        {
            comb[x][y] = ' ';
        }
    }

    for (unsigned x = 0; x < 8; ++x)
    {
        for (unsigned y = 0; y < 8; ++y)
        {
            cancel[x][y] = false;
        }
    }

    unsigned C, D;

    fi >> C;
    for (unsigned ci = 0; ci < C; ++ci)
    {
        string s3;
        fi >> s3;
        const char *cs3 = s3.c_str();
        comb_set(cs3[0], cs3[1], cs3[2]);
    }

    fi >> D;
    for (unsigned di = 0; di < D; ++di)
    {
        string s2;
        fi >> s2;
        const char *cs2 = s2.c_str();
        cancel_set(cs2[0], cs2[1]);
    }

    fi >> N;
    fi >> cseq_in;
}

void Magicka::solve()
{
    cseq_out.push_back(cseq_in[0]);
    for (unsigned ci = 1; ci < N; ++ci)
    {
        char c = cseq_in[ci];
        char cnew = (cseq_out.empty() ? ' ' : comb_get(cseq_out.back(), c));
        if (cnew == ' ')
        {
            // check for cancel
            bool to_cancel = false;
            for (auto oi = cseq_out.begin(), oe = cseq_out.end();
                (oi != oe) && !to_cancel; ++oi)
            {
                to_cancel = cancel_get(c, *oi);
            }
            if (to_cancel)
            {
                cseq_out.clear();
            }
            else
            {
                cseq_out.push_back(c);
            }
        }
        else
        {
            cseq_out.back() = cnew;
        }
    }
}

void Magicka::print_solution(ostream &fo)
{
    const char *sep = "";
    fo << " [";
    for (auto oi = cseq_out.begin(), oe = cseq_out.end(); oi != oe; ++oi)
    {
        fo << sep << *oi;
        sep = ", ";
    }
    fo << "]";
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

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Magicka magicka(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        magicka.solve();
        fout << "Case #"<< ci+1 << ":";
        magicka.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

