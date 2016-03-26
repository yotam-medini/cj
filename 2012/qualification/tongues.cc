// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

static unsigned dbg_flags;

static void learn(char *perm)
{
    static const char *Googlerese =
"ejp mysljylc kd kxveddknmc re jsicpdrysi"
"rbcpc ypc rtcsra dkh wyfrepkym veddknkmkrkcd"
"de kr kd eoya kw aej tysr re ujdr lkgc jv";

    static const char *English =
"our language is impossible to understand"
"there are twenty six factorial possibilities"
"so it is okay if you want to just give up";

    if (strlen(Googlerese) != strlen(English))
    {
        cerr << "Fail strlen= @" << __LINE__ << "\n";
        exit(1);
    }

    fill_n(perm, 26, ' ');
    for (unsigned ci = 0, nc = strlen(Googlerese); ci < nc; ++ci)
    {
        char cg = Googlerese[ci];
        char ce = English[ci];

        if (cg != ' ')
        {
            int pi = cg - 'a';
            if (pi < 0 || pi >= 26)
            {
                 cerr << "Non lowe case @" << __LINE__ << "\n";
                 exit(1);
            }
            if (perm[pi] != ' ' && perm[pi] != ce)
            {
                 cerr << "Inconsistent " << cg << " -> {" << 
                     perm[pi] << "," << ce << "}\n @" << __LINE__ << "\n";
            }
            perm[pi] = ce;
        }
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

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    char perm[26];
    learn(perm);

    for (int ai = 4; ai < argc; ++ai)
    {
       const char *s2 = argv[ai];
       if (strlen(s2) == 2)
       {
           char cg = s2[0], ce = s2[1];
           cerr << cg << " -> " << ce << "\n";
           perm[cg - 'a'] = ce;
       }
    }

    unsigned n_cases;
    *pfi >> n_cases;
    pfi->getline(0, 0);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases && !pfi->eof(); ci++)
    {
        string gline;
        getline(*pfi, gline);
        const char *cs_gline = gline.c_str();

        string eline;        
        for (unsigned ci = 0, nc = strlen(cs_gline); ci < nc; ++ci)
        {
            char cg = cs_gline[ci], ce = ' ';
            if (cg != ' ')
            {
                int pi = cg - 'a';
                if (pi < 0 || pi >= 26)
                {
                     cerr << "Non lowe case @" << __LINE__ << "\n";
                     exit(1);
                }
                ce = perm[pi];
                if (ce == ' ')
                {
                    ce = '?';
                }
            }
            eline += (ce != '?' ? ce : toupper(cg));
        }
        fout << "Case #" << (ci + 1) << ": " << eline << "\n";
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

