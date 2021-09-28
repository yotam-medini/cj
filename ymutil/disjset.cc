// Disjoint sets
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2021/September/25

#include <list>
#include <unordered_map>
using namespace std;

typedef unsigned u_t;

template <typename T> class SizedList; // fwd

template <typename T>
using list_pszl_t = list<SizedList<T>*>;

template <typename T> class DisjointSetElement;

template <typename T>
class DisjointSetElement
{
 public:
    DisjointSetElement(const T& _v, SizedList<T>* _p) : v(_v), mylist(_p) {}
    T v;
    SizedList<T>* mylist;
};

template <typename T>
class SizedList
{
 public:
    typedef DisjointSetElement<T> element_t;
    typedef element_t *elementp_t;
    typedef list<elementp_t> list_t;
    SizedList<T>() : sz(0) {}
    elementp_t push_back(const T &e)
    {
        elementp_t setp = new element_t(e, this);
        l.push_back(setp);
        ++sz;
        return setp;
    }
    list_t l;
    u_t sz;
    typename list_pszl_t<T>::iterator selfref;
};

class DisjointSets
{
 public:
    typedef u_t key_t;
    typedef SizedList<key_t> szlist_t;
    typedef szlist_t::elementp_t elementp_t;
    // typedef unordered_map<key_t, szlist_t*> k2l_t;
    // typedef list<szlist_t*> lszlp_t;
    typedef list_pszl_t<key_t> lszlp_t;

    virtual ~DisjointSets()
    {
        for (szlist_t *pszl: list_szlistp)
        {
            for (elementp_t e: pszl->l)
            {
                delete e;
            }
            delete pszl;
        }
    }

    elementp_t make_set(const key_t& k)
    {
        szlist_t *pszl = new szlist_t();
        elementp_t elemp = pszl->push_back(k);
        // k2l.insert(k2l.end(), k2l_t::value_type(k, pszl));
        // krep2l.insert(krep2l.end(), k2l_t::value_type(k, pszl));
        pszl->selfref = list_szlistp.insert(list_szlistp.end(), pszl);
        return elemp;
    }

#if 0
    szlist_t* find_set(const key_t& k)
    {
#if 1
        k2l_t::iterator iter = k2l.find(k);
        szlist_t *pszl = (iter != k2l.end() ? iter->second : nullptr);
#endif
        return pszl;
    }
#endif

    szlist_t* find_set(const elementp_t& elemp)
    {
        return elemp->mylist;
    }

#if 0
    void union_sets(key_t k0, key_t k1)
    {
        szlist_t* pszl0 = find_set(k0);
        szlist_t* pszl1 = find_set(k1);
        if (pszl0 != pszl1)
        {
            if (pszl0->sz > pszl1->sz)
            {
                swap(pszl0, pszl1);
            }
            pszl0->l.splice(pszl0->l.end(), pszl1->l);
            pszl0->sz += pszl1->sz;
            list_szlistp.erase(pszl1->selfref);
            delete pszl1;
        }
    }
#endif

    void union_sets(elementp_t e0, elementp_t e1)
    {
        szlist_t* pszl0 = find_set(e0);
        szlist_t* pszl1 = find_set(e1);
        if (pszl0 != pszl1)
        {
            if (pszl0->sz > pszl1->sz)
            {
                swap(pszl0, pszl1);
            }
            for (elementp_t e: pszl1->l)
            {
                e->mylist = pszl0;
            }
            pszl0->l.splice(pszl0->l.end(), pszl1->l);
            pszl0->sz += pszl1->sz;
            list_szlistp.erase(pszl1->selfref);
            delete pszl1;
        }
    }

    const lszlp_t& get_list_lists() const { return list_szlistp; }

 private:
#if 0
    key_t key_rep(const key_t& k) const
    {
        szlist_t* pszl = k2l.find(k)->second;
        szlist_t::list_t::const_iterator iter = pszl->l.begin();
        return iter->v;
    }
    k2l_t k2l;
    k2l_t krep2l;
#endif
    lszlp_t list_szlistp;
};

////////////////////////////////////////////////////////////////////////
// Test
////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <vector>
#include <set>
typedef array<u_t, 2> au2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<au2_t> vau2_t;

#include <unordered_map>

static void add_edges_spanning_set(vau2_t& edges, const vu_t& vset)
{
    const u_t n = vset.size();
    set<au2_t> edges_set;
    set<u_t> pre_linked;
    while (pre_linked.size() + 1 < n)
    {
        au2_t ij;
        ij[0] = ij[1] = rand() % n;
        while (ij[1] == ij[0])
        {
            ij[1] = rand() % n;
        }
        if (ij[0] > ij[1])
        {
            swap(ij[0], ij[1]);
        }
        edges_set.insert(au2_t{vset[ij[0]], vset[ij[1]]});
        pre_linked.insert(ij[1]);
    }
    edges = vau2_t(edges_set.begin(),  edges_set.end());
}

static int disj_equal(const vvu_t& vsets, const DisjointSets& djsets)
{
    vvu_t sorted_vsets, djsets_vsets;

    for (vu_t vset: vsets)
    {
        sort(vset.begin(), vset.end());
        sorted_vsets.push_back(vset);
    }    
    sort(sorted_vsets.begin(), sorted_vsets.end());

    for (const DisjointSets::szlist_t* szlist: djsets.get_list_lists())
    {
        vu_t vset;
        for (const DisjointSets::elementp_t e: szlist->l)
        {
            u_t v = e->v;
            vset.push_back(v);
        }
        sort(vset.begin(), vset.end());
        djsets_vsets.push_back(vset);
    }
    sort(djsets_vsets.begin(), djsets_vsets.end());
    int rc = ((sorted_vsets == djsets_vsets) ? 0 : 1);
    return rc;
}   

static int test_specific(const vvu_t& vsets) // assuming disjoing!
{
    int rc = 0;
    DisjointSets djsets;
    typedef unordered_map<u_t, DisjointSets::elementp_t> u2e_t;
    u2e_t u2e;
    vau2_t edges;

    for (const vu_t& vset: vsets)
    {
        for (u_t u: vset)
        {
            DisjointSets::elementp_t elemp = djsets.make_set(u);
            u2e.insert(u2e.end(), u2e_t::value_type(u, elemp));
            add_edges_spanning_set(edges, vset);
        }
    }

    while (!edges.empty())
    {
        u_t i = rand() % edges.size();
        au2_t edge = edges[i];
        edges[i] = edges.back();
        edges.pop_back();
        if (rand() % 2 != 0)
        {
            swap(edge[0], edge[1]);
        }
        djsets.union_sets(u2e[edge[0]], u2e[edge[1]]);
    }

    rc = disj_equal(vsets, djsets);
    if (rc != 0)
    {
        cerr << "Failed:\nspecific";
        for (const vu_t& vset: vsets)
        {
            for (u_t u: vset)
            {
                cerr << ' ' << u;
            }
            cerr << -1;
        }
        cerr << '\n';
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if ((argc > 1) && (string(argv[1]) == string("specific")))
    {
        vvu_t vsets;
        vu_t vset;
        for (int ai = 2; ai < argc; ++ai)
        {
            int n = stoi(argv[ai]);
            if (n >= 0)
            {
                vset.push_back(n);
            }
            else if (!vset.empty())
            {
                vsets.push_back(vset);
                vset.clear();
            }
        }
        if (!vset.empty())
        {
            vsets.push_back(vset);
        }
        rc = test_specific(vsets);
    }
    return rc;
}
