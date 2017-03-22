// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

typedef const char* pchar_t;
typedef set<string> setstr_t;
typedef vector<setstr_t> vsetstr_t;
typedef vector<double> vdbl_t;

static unsigned dbg_flags;

class Node
{
 public:
    Node(double p=0., const string& f="") :
       probability(p),
       feature(f),
       child{0, 0}
    {}
    ~Node()
    {
        delete child[0];
        delete child[1];
    }
    double probability;
    string feature;
    Node* child[2];
};
typedef Node *pNode_t;

class Decision
{
 public:
    Decision(istream& fi);
    ~Decision()
    {
        delete root;
    }
    void solve();
    void print_solution(ostream&) const;
 private:
    void get_node(pNode_t &node, pchar_t &text);
    void print_node(const Node* node, const string& indent="") const;
    double solve_animal(unsigned i, const Node *node, double p) const;
    Node *root;
    vsetstr_t animals_features;    
    vdbl_t solution;
};

Decision::Decision(istream& fi) : root(0), solution(0.)
{
    unsigned Nlines;
    fi >> Nlines;
    string dumnl;
    getline(fi, dumnl);
    string tree_text;
    for (unsigned n = 0; n < Nlines; ++n)
    {
        string line;
        getline(fi, line);
        tree_text += line;
    }
    const char *cs_tree_text = tree_text.c_str();
    get_node(root, cs_tree_text);
    if (dbg_flags & 0x1)
    {
        print_node(root);
    }

    fi >> Nlines;
    for (unsigned n = 0; n < Nlines; ++n)
    {
        string animal;
        fi >> animal; // whi cares?
        unsigned nf;
        fi >> nf;
        setstr_t features;
        for (unsigned fei = 0; fei < nf; ++fei)
        {
            string feature;
            fi >> feature;
            features.insert(features.end(), feature);
        }
        animals_features.push_back(features);
    }
}

void Decision::get_node(pNode_t &node, pchar_t &text)
{
    while (*text != '\0' && *text != '(')
    {
        ++text;
    }
    if (*text != '(')
    {
        cerr << "Error @" << __LINE__ << "\n";
    }
    ++text;
    char *etext;
    double p = strtod(text, &etext);
    text = etext;
    node = new Node(p);
    while (*text == ' ' && *text != '\0')
    {
        ++text;
    }
    if (*text == '\0')
    {
        cerr << "Error @" << __LINE__ << "\n";
    }
    if (*text != ')')
    {
        char buf[81];
        sscanf(text, "%s", buf);
        text += strlen(buf);
        node->feature = string(buf);
        get_node(node->child[0], text);
        get_node(node->child[1], text);
    }
    while (*text != '\0' && *text != ')')
    {
        ++text;
    }
    if (*text != ')')
    {
        cerr << "Error @" << __LINE__ << "\n";
    }
    ++text; // skip ')'
}

void Decision::print_node(const Node* node, const string& indent) const
{
    cerr << indent << "(" << node->probability;
    if (node->child[0])
    {
        string subindent = indent + string("  ");
        cerr << " " << node->feature << "\n";
        print_node(node->child[0], subindent);
        print_node(node->child[1], subindent);
        cerr << indent;
    }
    cerr << ")\n";
}

void Decision::solve()
{
    for (unsigned i = 0, n = animals_features.size(); i < n; ++i)
    {
        double p = solve_animal(i, root, 1.);
        solution.push_back(p);
    }
}

double Decision::solve_animal(unsigned i, const Node *node, double p) const
{
    const setstr_t &features = animals_features[i];
    p *= node->probability;
    if (node->child[0])
    {
        auto w = features.find(node->feature);
        unsigned ci = w != features.end() ? 0 : 1;
        p = solve_animal(i, node->child[ci], p);
    }
    return p;
}

void Decision::print_solution(ostream &fo) const
{
    fo << "\n";
    for (auto i = solution.begin(); i != solution.end(); ++i)
    {
        char buf[0x10];
        sprintf(buf, "%.7f", *i);
        fo << buf << "\n";
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

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Decision problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        // fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
