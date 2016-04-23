#if !defined(GET_PRIMES_H)
#define GET_PRIMES_H 1

#include <vector>

extern const std::vector<unsigned long> &get_primes(
    unsigned long at_least_until);

#endif /* GET_PRIMES_H */
