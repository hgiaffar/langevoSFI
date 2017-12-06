#ifndef H5_DATASPACE_HPP
#define H5_DATASPACE_HPP

#include <H5Cpp.h>
#include <type_traits>
#include <cstring>
#include "sccs.h++"
#include "myutil.h++"

namespace H5Util {

  static const SCCS::sccs_id H5_DATASPACE_HPP_SCCS_ID __attribute__((used)) = "@(#)h5_dataspace.h++: $Id$";

  inline const H5::DataSpace& seqspace(void) {
    // H5S_UNLIMITED is a macro; namespace pollution
    static const hsize_t zero[] = {0}, infinity[] = {H5S_UNLIMITED};
    static const H5::DataSpace retval(1,zero,infinity);
    return retval;
  }
  inline const H5::DataSpace& scalarspace(void) {
    static const H5::DataSpace retval(H5S_SCALAR);
    return retval;
  }

  declare_member_check(has_h5dims,h5dims)

  template<typename T,
	   typename std::enable_if<decltype(has_h5dims(std::declval<T*>())):value,int>::type=0>
  inline decltype(declval<T>.h5dims()) h5dims(const T& v) {
    return v.h5dims();
  }
  
  template<typename T,
	   typename std::enable_if<decltype(util::has_base_template<std::vector>(std::declval<T*>()))::value &&
				   !decltype(has_h5dims(std::declval<T*>()))::value,int>::type=0>
  inline std::vector<hsize_t> h5dims(const std::vector<T>& v) {
    std::vector<hsize_t> r(h5dims(v));
    r.insert(r.begin(),r.size());
    return r;
  }
  
  template<typename T, typename std::enable_if<!decltype(util::has_base_template<std::vector>(std::declval<T*>()))::value &&
					       !decltype(has_h5dims(std::declval<T*>))::value,int>::type=0>
  inline std::vector<hsize_t> h5dims(const std::vector<T>& v) {
    static std::vector<hsize_t> r(1,1);
    return r;
  }

  inline H5::DataSpace choosespace(const std::vector<hsize_t>& s) {
    assert(s.back()==1);
    s.pop_back();
    return s.empty()?scalarsapce():H5::DataSpace(s.size(),s,s);
  }

}
#endif
