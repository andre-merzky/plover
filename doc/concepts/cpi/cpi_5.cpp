
#include "cpi_5.hpp"

static saga::util::shared_ptr <saga::impl::engine> the_engine (new saga::impl::engine);

namespace saga
{
  namespace impl
  {
    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      file::file (void)                    // FIXME: the file impl and pimpl object hierarchies are ignored
        : engine_ (the_engine            ) // FIXME: no need to create a new engine - get it from the session!
        , idata_  (new file_instance_data)
      {
        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::constructor (std::string url)
      {
        saga::util::scoped_lock sl (idata_);

        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                      void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        return engine_->call <api_t, cpi_t, void_t> (cc);
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

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        return engine_->call <api_t, cpi_t, int> (cc);
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::copy, tgt));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        return engine_->call <api_t, cpi_t, void_t> (cc);
      }




      //////////////////////////////////////////////////////////////////
      dir::dir (void)                     // FIXME: the dir impl and pimpl object hierarchies are ignored
        : engine_ (the_engine           ) // FIXME: no need to create a new engine - get it from the session!
        , idata_  (new dir_instance_data)
      {
        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t dir::constructor (std::string url)
      {
        saga::util::scoped_lock sl (idata_);
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::filesystem::dir                 api_t;
        typedef saga::impl::filesystem::dir_cpi             cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        return engine_->call <api_t, cpi_t, void_t> (cc);
      }

      //////////////////////////////////////////////////////////////////
      std::string dir::get_url (void)
      {
        typedef saga::impl::filesystem::dir                       api_t;
        typedef saga::impl::filesystem::dir_cpi                   cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::get_url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        return engine_->call <api_t, cpi_t, std::string> (cc);
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga



int main ()
{
  try
  {
    for ( unsigned int i = 0; i < 1; i++ )
    {
    saga::filesystem::file f ("/etc/passwd");
    std::cout << "file size: " << f.get_size () << std::endl;
    f.copy ("/tmp/passwd.bak");

    saga::filesystem::dir d ("/etc/");
    std::cout << "dir url: " << d.get_url () << std::endl;
    }
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

