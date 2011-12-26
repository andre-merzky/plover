
//  Copyright (c) 2005-2006 Andre Merzky (andre@merzky.net)
// 
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#ifndef SAGA_UTIL_UTILS_HPP
#define SAGA_UTIL_UTILS_HPP

#include <string>
#include <vector>

namespace saga
{
    namespace util
    {
      std::vector <std::string> split     (std::string               const & line,
                                           char                      const & delim,
                                           int                       const & number = -1 );
      std::vector <std::string> split     (std::string               const & line,
                                           std::string               const & delims = "\n\t;,. ",
                                           int                       const & number = -1 );
      std::vector <std::string> split_cmd (std::string               const & line);
      std::vector <std::string> head      (std::size_t               const & n, 
                                           std::vector <std::string> const & in);
      std::vector <std::string> tail      (std::size_t               const & n,
                                           std::vector <std::string> const & in);
      std::vector <std::string> rev       (std::vector <std::string> const & in);
      std::vector <std::string> tac       (std::vector <std::string> const & in);
      bool                      find      (std::string               const & pattern, 
                                           std::string               const & in);
      std::vector <std::string> grep      (std::string               const & pattern, 
                                           std::vector <std::string> const & in);
      std::string               to_lower  (std::string               const & in);
      std::string               to_upper  (std::string               const & in);
      bool                      icompare  (std::string               const & a,
                                           std::string               const & b);
      std::string               itoa      (long long int                     in);

    } // namespace util 

} // namespace saga

#endif // SAGA_UTIL_UTILS_HPP

