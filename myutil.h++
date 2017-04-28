#ifndef MYUTIL_HPP
#define MYUTIL_HPP

#include <cmath>
#include <random>

static const char MYUTIL_HPP_SCCS_ID[] __attribute__((used)) = "@(#)myutil.h++: $Id$";

const double infinity=1.e100;
inline double probit(const double p) {
  return p<=0.0?-infinity:p>=1.0?infinity:log(p/(1-p));
}
inline double invprobit(const double probit) {
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

  const double m = sqrt(-2*log(s)/s);

  z0 = u1 * m;
  z1 = u2 * m;
  return z0 * sigma + mu;
}

template<class RandomIt, class UniformRandomBitGenerator>
void myshuffle(RandomIt first, RandomIt last,
	       UniformRandomBitGenerator& g)
{
  const auto n = last - first;
  for (auto i = n-1; i > 0; --i) {
    using std::swap;
    swap(first[i], first[i*std::generate_canonical<double,20>(g)]);
  }
}

// Determine how many bits two integers have in common
// (If they don't come unsigned, they will me made unsigned)
unsigned common_bits(int b1, int b2) { 
	unsigned x = static_cast<unsigned>(b1) ^ static_cast<unsigned>(b2); // Find the xor of b1, b2; now count the number of bits that are clear
	unsigned m = 0;
	while(x) {
		if((x & 1) == 0) m++;
		x>>=1; // Shift one bit to the right
	} // There is probably a more efficient way to do this...
	return m;
}

// Count how many bits are required to store the set of numbers 0, 1, ..., n-1
// Note if n<=0, we will get 0.
// (If everyone used unsigned in preference to int habitually, this would never happen)
unsigned count_bits(int n) {
	unsigned b = 0;
	while(1<<b < n) ++b;
	return b;
}
		


#endif
