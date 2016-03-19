// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

static unsigned dbg_flags;


class Water
{
 public:
    enum {
        W_MAX = 100,
        H_MAX = 100
    };
    Water(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    unsigned w, h;
    unsigned alt[W_MAX][H_MAX];
    char label[W_MAX][H_MAX];

    char flood(int x, int y, char cc);
    bool next_decend(int &x_next, int &y_next, int x, int y);
};

Water::Water(istream& fi)
{
    fi >> h >> w;
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            fi >> alt[x][y];
            label[x][y] = ' ';
        }
    }
}

void Water::solve()
{
    char cc = 'a';
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            cc = flood(x, y, cc);
        }
    }
}

char Water::flood(int x0, int y0, char cc)
{
    char ret = cc;
    if (label[x0][y0] == ' ')
    {
        char sink = '?';
        for (unsigned pass = 0; pass < 2; ++pass)
        {
            int x = x0, y = y0;
            bool decend = true;
            while (decend) 
            {
                if (pass == 1)
                {
                    label[x][y] = sink;
                }

                int x_next, y_next;
                decend = next_decend(x_next, y_next, x, y);
                if (decend && label[x_next][y_next] != ' ')
                {
                    sink = label[x_next][y_next];
                    decend = false;
                }
                else
                {
                    if (decend)
                    {
                        x = x_next;
                        y = y_next;
                    }
                    else if (pass == 0)
                    {
                        if (label[x][y] == ' ')
                        {
                            sink = cc;
                            label[x][y] = sink;
                            ret = char(int(cc) + 1);
                        }
                        else
                        {
                            sink = label[x][y];
                        }
                    }
                }
            }
        }
    }
    return ret;
}

bool Water::next_decend(int &x_next, int &y_next, int x, int y)
{
    unsigned alt_xy = alt[x][y];
    unsigned alt_lowest = alt_xy;
    for (unsigned si = 0; si < 4; ++si)
    {
        static const  int steps[4][2] =
        {
            {0, -1}, // N
            {-1, 0}, // W
            {1, 0},  // E
            {0, 1}   // S
        };

        int x_nbr = x + steps[si][0];
        int y_nbr = y + steps[si][1];
        if (0 <= x_nbr && x_nbr < int(w) &&
            0 <= y_nbr && y_nbr < int(h) &&
            alt[x_nbr][y_nbr] < alt_lowest)
        {
            alt_lowest = alt[x_nbr][y_nbr];
            x_next = x_nbr;
            y_next = y_nbr;
        }
    }
    bool decend = (alt_lowest < alt_xy); //  && label[x_next][y_next] == ' ';
    return decend;
}


void Water::print_solution(ostream &fo)
{
    fo << "\n";
    for (unsigned y = 0; y < h; ++y)
    {
        const char *space = "";
        for (unsigned x = 0; x < w; ++x)
        {
            fo << space << label[x][y];
            space = " ";
        }
        fo << "\n";
    }
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
        Water problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

