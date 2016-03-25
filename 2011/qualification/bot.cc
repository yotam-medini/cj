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

class Task
{
 public:
  Task(unsigned int vc=' ', int vb=0) : color(vc), button(vb) {}
  unsigned int color; // O=0, B=1
  int button;
};

typedef vector<Task> vtask_t;

class Bot
{
 public:
    Bot(istream& fi);
    void solve();
    void print_solution(ostream&);
 private:
    int solution;
    unsigned N;
    vtask_t tasks;
};

Bot::Bot(istream& fi) : solution(0)
{
    fi >> N;
    for (unsigned i = 0; i < N; ++i)
    {
        string s;
        int b;
        fi >> s >> b;
        const char *cs = s.c_str();
        unsigned vc = (cs[0] == 'O' ? 0 : 1);
        tasks.push_back( Task(vc, b) );
    }
}

void Bot::solve()
{
    int positions[2];
    unsigned next_ti[2];
    unsigned ti, c_first;

    positions[0] = positions[1] = 1;

    // initialize next task indices
    for (unsigned c = 0; c < 2; ++c)
    {
        for (ti = 0; (ti < N) && tasks[ti].color != c; ++ti) {};
        next_ti[c] = ti;
    }

    while ((next_ti[0] < N) || (next_ti[1] < N))
    {
        if ((next_ti[0] < N) && (next_ti[1] < N))
        {
            int moves[2], a_moves[2];
            for (unsigned c = 0; c < 2; ++c)
            {
                moves[c] = tasks[next_ti[c]].button - positions[c];
                a_moves[c] = abs(moves[c]);
            }
            // c_first = (a_moves[0] <= a_moves[1] ? 0 : 1);
            c_first = (next_ti[0] <= next_ti[1] ? 0 : 1);
            unsigned c_second = 1 - c_first;
            positions[c_first] = tasks[next_ti[c_first]].button;
            if (a_moves[0] == a_moves[1])
            {
                positions[c_second] = tasks[next_ti[c_second]].button;
            }
            else
            {
                int advance = min(a_moves[c_first] + 1, a_moves[c_second]);
                if (moves[c_second] < 0)
                {
                    advance = (-advance);
                }
                positions[c_second] += advance;
            }
            solution += a_moves[c_first] + 1;
        }
        else 
        {
            c_first = (next_ti[0] < N ? 0 : 1);
            int move = tasks[next_ti[c_first]].button - positions[c_first];
            int a_move = abs(move);
            positions[c_first] = tasks[next_ti[c_first]].button;
            solution += a_move + 1;
        }

        for (ti = next_ti[c_first] + 1;
            (ti < N) && tasks[ti].color != c_first; ++ti) {};
        next_ti[c_first] = ti;
    }
    
}

void Bot::print_solution(ostream &fo)
{
    fo << ' ' << solution;
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
        Bot bot(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        bot.solve();
        fout << "Case #"<< ci+1 << ":";
        bot.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

