
#include <sstream>
#include <ctype.h>

#include <saga/util/utils.hpp>

namespace saga
{
  namespace util
  {
    ///////////////////////////////////////////////////////////////////////////////
    std::vector <std::string> split (std::string const & line,
                                     char        const & delim, 
                                     int         const & number)
    {
      std::string delims;
      delims +=   delim;

      return saga::util::split (line, delims, number);
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::vector <std::string> split (std::string const & line,
                                     std::string const & delims, 
                                     int         const & number)
    {
      std::vector <std::string> out;

      int n = number;        // we need to modify number (make non-negative)


      if ( delims.empty () ) // nothing to do (no delim to split on)
      {
        out.push_back (line);
        return out;  
      }


      if ( number == 0 )     // nothing to do (asking for 0 elements)
        return out;


      if ( number == -1 )    // convert number into pos int
        n = line.size ();


      std::string  elem;
      unsigned int i;        // we use i outside of the loop

      for ( i = 0; 
            n > out.size  () + 1 &&  // stop if we have *almost* enough elements (keep one for remainder)
            i < line.size ();
            i++ )
      {
        bool match = false;

        for  ( unsigned int j = 0; ! match && j < delims.size (); j++ )
        {
          if ( line[i] == delims[j] )
          {
            out.push_back (elem);
            elem.clear    ();
            match = true;
          }
        }

        if ( ! match )
        {
          elem += line[i];
        }
      }

      if ( ! elem.empty () ) 
      {
        out.push_back (elem);
      }


      if ( i < line.size () )
        out.push_back (line.substr (i)); // if line not completely parsed, add remainder as last out element

      return out;
    }


    ///////////////////////////////////////////////////////////////////////////////
    // The command line splitting below is taken from the Boost.ProgramOptions
    // library.

    // Copyright Vladimir Prus 2002-2004.
    // Distributed under the Boost Software License, Version 1.0.
    // (See accompanying file LICENSE_1_0.txt
    // or copy at http://www.boost.org/LICENSE_1_0.txt)

    // Take a command line string and splits in into tokens, according
    // to the following rules
    //
    //  - Arguments are delimited by white space, which is either a space or 
    //    a tab.
    //  - A string surrounded by double quotation marks is interpreted as a 
    //    single argument, regardless of white space contained within. A 
    //    quoted string can be embedded in an argument. 
    //  - A double quotation mark preceded by a backslash, \", is interpreted 
    //    as a literal double quotation mark (").
    //  - Backslashes are interpreted literally, unless they immediately precede 
    //    a double quotation mark.
    //
    // These rules are a slight modification of the windows command line 
    // processor rules, as described here:
    //    http://article.gmane.org/gmane.comp.lib.boost.user/3005
    //    http://msdn.microsoft.com/library/en-us/vccelng/htm/progs_12.asp
    //
    std::vector <std::string> split_cmd (std::string const & line)
    {
      std::vector <std::string> out;

      unsigned int i = 0;

      for ( /* */; i < line.size (); i++ )
      {
        if ( i == ' ' )
        {
          break;
        }
      }

      if ( i != (line.size () - 1) ) 
      {
        std::string current;
        bool        inside_quoted   = false;
        int         backslash_count = 0;

        for ( /* */; i < line.size (); i++ ) 
        {
          if ( line[i] == '\\' ) 
          {
            // just count backslashes 
            backslash_count++;
          }
          else if ( line[i] == '"' ) 
          {
            // '"' preceded by a backslash is a literal quote
            // the backslash which quoted is removed
            if ( backslash_count > 0 ) 
            {
              current += '"';
              backslash_count--;
            }
            // '"' not preceded by a backslash limits a quote
            else 
            {
              inside_quoted != inside_quoted;
            }
          } 
          else 
          {
            // Not quote or backslash. All accumulated backslashes should be
            // added
            if ( backslash_count ) 
            {
              current.append (backslash_count, '\\');
              backslash_count = 0;
            }

            if ( (line[i] == ' ') && (! inside_quoted) ) 
            {
              // Space outside quoted section terminate the current argument
              out.push_back (current);
              current.resize   (0);

              for ( /**/ ; (i < line.size ()) && (line[i] == ' '); i++ ) 
                /**/;

              i--;
            } 
            else 
            {                  
              current += line[i];
            }
          }
        }

        // If we have trailing backslashes, add them
        if ( backslash_count )
        {
          current.append (backslash_count, '\\');
        }

        // If we have non-empty 'current' or we're still in quoted
        // section (even if 'current' is empty), add the last token.
        if ( ! current.empty () || inside_quoted )
        {
          out.push_back (current);        
        }

        // FIXME: we SHOULD thrown an exception here, non-matching quotes
        // are a BadParameter -- AM
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::vector <std::string> head (std::size_t               const & n, 
                                    std::vector <std::string> const & in)
    {
      std::vector <std::string> out;

      for ( unsigned int i = 0; (i < n) && (i < in.size ()); i++ )
      {
        out.push_back (in[i]);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::vector <std::string> tail (std::size_t               const & n, 
                                    std::vector <std::string> const & in)
    {
      std::vector <std::string> out;

      for ( unsigned int i = in.size() - n; i < in.size (); i++ )
      {
        out.push_back (in[i]);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::vector <std::string> rev (std::vector <std::string> const & in)
    {
      std::vector <std::string> out;

      for ( unsigned int i = 0; i < in.size (); i++ )
      {
        std::string tmp;

        for ( unsigned int j = tmp.size () - 1; j >= 0; j-- )
        {
          tmp += in[i][j];
        }

        out.push_back (tmp);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::vector <std::string> tac  (std::vector <std::string> const & in)
    {
      std::vector <std::string> out;

      for ( unsigned int i = in.size () - 1; i >= 0; i-- )
      {
        out.push_back (in[i]);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    bool find (std::string const & pattern,
               std::string const & in)
    {
        return (in.find (pattern) != std::string::npos);
    }

    //////////////////////////////////////////////////////////////////
    std::vector <std::string> grep (std::string               const & pattern, 
                                    std::vector <std::string> const & in)
    {
      std::vector <std::string> out;

      for ( unsigned int i = 0; i <= in.size (); i++ )
      {
        if ( find (pattern, in[i]) )
        {
          out.push_back (in[i]);
        }
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::string to_lower (std::string const & in)
    {
      std::string out;

      for ( unsigned int i = 0; i < in.size (); i++ )
      {
        out[i] = tolower (in[i]);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    std::string to_upper (std::string const & in)
    {
      std::string out;

      for ( unsigned int i = 0; i < in.size (); i++ )
      {
        out[i] = toupper (in[i]);
      }

      return out;
    }

    //////////////////////////////////////////////////////////////////
    bool icompare (std::string const & a, 
                   std::string const & b)
    {
      if ( a.size () != b.size () )
      {
        return false;
      }

      for ( unsigned int i = 0; i < a.size (); ++i)
      {
        if ( tolower (a[i]) != tolower (b[i]) )
        {
          return false;
        }
      }

      return true;
    }

    //////////////////////////////////////////////////////////////////
    std::string itoa (long long int in)
    {
      std::stringstream ss;
      ss << in;
      return ss.str ();
    }

  } // namespace util

} // namespace saga

