// -*- c++ -*-
#if !defined(lInEq_H)
#define lInEq_H 1

#include <vector>
#include "matrix.h"

typedef double lineq_scalar_t;
typedef std::vector<lineq_scalar_t> vscalar_t;
typedef Matrix<lineq_scalar_t> lineq_mtx_t;

extern const lineq_scalar_t  lineq_eps;

extern
bool matvec_mult(vscalar_t &result, const lineq_mtx_t &m, const vscalar_t &v);

extern
bool lineq_solve(vscalar_t &x, const lineq_mtx_t &a, const vscalar_t &b);

#endif /* lInEq_H */
