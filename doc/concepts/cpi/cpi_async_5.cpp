
#include "cpi_5.hpp"


namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      file::file (void)                    // FIXME: the file impl and pimpl object hierarchies are ignored
        : engine_ (new saga::impl::engine) // FIXME: no need to create a new engine - get it from the session!
        , idata_  (new file_instance_data)
      {
        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void file::constructor (std::string url)
      {
        saga::util::scoped_lock sl (idata_->get_mutex ());
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        saga::impl::functor_1 <saga::impl::filesystem::file, 
                               saga::impl::filesystem::file_cpi, void, std::string>  
                        func (&saga::impl::filesystem::file_cpi::constructor, url);

        engine_->call <saga::impl::filesystem::file_cpi, 
                       saga::impl::filesystem::file, void> (func, x_get_shared_ptr <saga::impl::filesystem::file> ());
      }

      //////////////////////////////////////////////////////////////////
      int file::get_size (void)
      {
        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::impl::functor_0 <saga::impl::filesystem::file, 
                               saga::impl::filesystem::file_cpi, int> 
                        func (&saga::impl::filesystem::file_cpi::get_size);

        // the functor is given to the engine, so it can be used to call that
        // function on some cpi
        return engine_->call  <saga::impl::filesystem::file_cpi,
                               saga::impl::filesystem::file, int> (func, x_get_shared_ptr <saga::impl::filesystem::file> ());
      }

      //////////////////////////////////////////////////////////////////
      void file::copy (std::string tgt)
      {
        saga::impl::functor_1 <saga::impl::filesystem::file, 
                               saga::impl::filesystem::file_cpi, void, std::string> 
                        func (&saga::impl::filesystem::file_cpi::copy, tgt);

        return engine_->call <saga::impl::filesystem::file_cpi, 
                              saga::impl::filesystem::file, void> (func, x_get_shared_ptr <saga::impl::filesystem::file> ());
      }
    } // namespace filesystem

  } // namespace impl

} // namespace saga



int main ()
{
  try
  {
    saga::filesystem::file f ("/etc/passwd");
    std::cout << "size: " << f.get_size () << std::endl;
    f.copy ("/tmp/passwd.bak");
  }
  catch ( const std::exception & e )
  {
    std::cerr << "exception: " << e.what () << std::endl;
    return 1;
  }
  catch ( const char * err )
  {
    std::cerr << "error: " << err << std::endl;
    return 1;
  }

  return 0;
}

