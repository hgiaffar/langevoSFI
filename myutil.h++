#ifndef MYUTIL_HPP
#define MYUTIL_HPP

#include <cmath>
#include <random>
#include <utility>

static const char MYUTIL_HPP_SCCS_ID[] __attribute__((used)) = "@(#)myutil.h++: $Id$";

constexpr const double infinity=1.e100;
inline auto probit(const double p) {
  return p<=0.0?-infinity:p>=1.0?infinity:std::log(p/(1-p));
}
inline auto invprobit(const double probit) {
  return probit<=-infinity?0:probit>=infinity?1:
    1/(1+std::exp(-probit));
}
template<typename generator>
double BoxMueller(const double mu, const double sigma, generator &r)
{
// Box Mueller transform in polar form
  const double epsilon = std::pow(2.,-20);

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

  const auto m = std::sqrt(-2*std::log(s)/s);

  z0 = u1 * m;
  z1 = u2 * m;
  return z0 * sigma + mu;
}

template<class RandomIt, class UniformRandomBitGenerator>
void myshuffle(RandomIt first, RandomIt last,
	       UniformRandomBitGenerator&& g)
{
  const auto n = last - first;
  for (auto i = n-1; i > 0; --i) {
    using std::swap;
    swap(first[i], first[i*std::generate_canonical<double,20>(g)]);
  }
}
#endif
