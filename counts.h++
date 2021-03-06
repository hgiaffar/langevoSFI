#ifndef COUNTS_HPP
#define COUNTS_HPP

#include "enumvector.h++"
#include <iostream>

namespace Counts {

static const char COUNTS_HPP_SCCS_ID[] __attribute__((used)) = "@(#)counts.h++: $Id$";

struct Counts {
  double success=0; int tries=0;
  void operator += (const double s) {
    success += s; tries++;
  }
  void operator += (const Counts c) {
    success += c.success; tries += c.tries;
  }
  constexpr double mean(void) const {return success/tries;}
};

inline
std::ostream& operator<< (std::ostream& o, const Counts c) {
  return o << c.success << "/" << c.tries << " ";
}

template<typename T>
void summarize(const Enumvector::Enumvector<T,Counts> &counts) {
  Counts totcounts;
  for (Counts c: counts) totcounts += c;
  std::cout << "Comprehension " << totcounts.mean() << std::endl;
}

}

#endif
