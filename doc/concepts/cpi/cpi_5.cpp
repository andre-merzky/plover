
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
      void_t file::constructor (std::string url)
      {
        saga::util::scoped_lock sl (idata_->get_mutex ());
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                      void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> 
              func (new func_t (&cpi_t::constructor, url));

        return engine_->call <api_t, cpi_t, void_t> (func, shared_this <api_t> ());
      }

      //////////////////////////////////////////////////////////////////
      int file::get_size (void)
      {
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, int> func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::get_size));

        // the functor is given to the engine, so it can be used to call that
        // function on some cpi
        return engine_->call  <api_t, cpi_t, int> (func, shared_this <api_t> ());
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::copy, tgt));

        return engine_->call <api_t, cpi_t, void_t> (func, shared_this <api_t> ());
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

