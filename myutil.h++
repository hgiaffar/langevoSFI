#ifndef MYUTIL_HPP
#define MYUTIL_HPP

#include <cmath>
#include <random>

static const char MYUTIL_HPP_SCCS_ID[] __attribute__((used)) = "@(#)myutil.h++: $Id$";

/*

This file uses some code from the http://en.cppreference.com/  That code was copylefted under the following terms.

The content is licensed under Creative Commons Attribution-Sharealike 3.0 Unported License (CC-BY-SA) and by the GNU Free Documentation License (GFDL) (unversioned, with no invariant sections, front-cover texts, or back-cover texts). That means that you can use this site in almost any way you like, including mirroring, copying, translating, etc. All we would ask is to provide link back to cppreference.com so that people know where to get the most up-to-date content. In addition to that, any modified content should be released under a equivalent license so that everyone could benefit from the modified versions.

*/

constexpr const double infinity=1.e100;
inline auto probit(const double p) {
  return p<=0.0?-infinity:p>=1.0?infinity:log(p/(1-p));
}
inline auto invprobit(const double probit) {
  return probit<=-infinity?0:probit>=infinity?1:
    1/(1+exp(-probit));
}
template<typename generator>
double BoxMueller(const double mu, const double sigma, generator &r)
{
// Box Mueller transform in polar form
  const double epsilon = pow(2.,-20);

  static double z0, z1;
  static bool generate = false;
  generate = !generate;

  if (!generate)
    return z1 * sigma + mu;

  double u1, u2, s;
  do
    {
      u1 = 2*std::generate_canonical<double,20>(r)-1;
      u2 = 2*std::generate_canonical<double,20>(r)-1;
      s = u1*u1 + u2*u2;
    }
  while ( s <= epsilon || s >= 1 );

  const auto m = sqrt(-2*log(s)/s);

  z0 = u1 * m;
  z1 = u2 * m;
  return z0 * sigma + mu;
}

template<class RandomIt, class UniformRandomBitGenerator>
void myshuffle(RandomIt first, RandomIt last,
	     UniformRandomBitGenerator&& g)
{
  typedef typename std::iterator_traits<RandomIt>::difference_type diff_t;
  typedef typename std::make_unsigned<diff_t>::type udiff_t;
  typedef typename std::uniform_int_distribution<udiff_t> distr_t;
  typedef typename distr_t::param_type param_t;

  distr_t D;
  diff_t n = last - first;
  for (diff_t i = n-1; i > 0; --i) {
    using std::swap;
    swap(first[i], first[D(g, param_t(0, i))]);
  }
}
#endif
