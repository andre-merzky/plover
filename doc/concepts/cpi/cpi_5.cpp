
#include <mcheck.h>

#include "cpi_5.hpp"

static saga::util::shared_ptr <saga::impl::engine> the_engine (new saga::impl::engine);

namespace saga
{
  namespace impl
  {
    task::task (void)
      : engine_ (the_engine            ) // FIXME: no need to create a new engine - get it from the session!
      , idata_  (new task_instance_data)
    {
    }

    task::task (saga::util::shared_ptr <saga::impl::call_context> cc)
      : engine_ (the_engine            ) // FIXME: no need to create a new engine - get it from the session!
      , idata_  (new task_instance_data)
    {
      saga::util::scoped_lock sl (idata_->get_mutex ());

      idata_->cc = cc;
    }

    void_t task::constructor (void)
    {
      saga::util::scoped_lock sl (idata_->get_mutex ());

      typedef saga::impl::task                            api_t;
      typedef saga::impl::task_cpi                         cpi_t;
      typedef saga::impl::functor_0 <api_t, cpi_t, void_t> func_t;

      saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::constructor));

      func.dump ("task::ctor functor");

      saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

      cc->set_func_name ("task::constructor");

      engine_->call <api_t, cpi_t, void_t> (cc);
    }


    saga::impl::call_state task::get_state (void)
    {
      saga::util::scoped_lock sl (idata_->get_mutex ());

      typedef saga::impl::task                                             api_t;
      typedef saga::impl::task_cpi                                         cpi_t;
      typedef saga::impl::functor_0 <api_t, cpi_t, saga::impl::call_state> func_t;

      saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::get_state));

      func.dump ("task::get_state functor");

      saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

      cc->set_func_name ("task::get_state");

      saga::impl::call_state ret = engine_->call <api_t, cpi_t, call_state> (cc);

      if ( cc->get_state () == Failed )
      {
        throw " task::get_state failed";
      }

      return ret;
    }

    result_t task::get_result (void)
    {
    }

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
        saga::util::scoped_lock sl (idata_->get_mutex ());

        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::filesystem::file                              api_t;
        typedef saga::impl::filesystem::file_cpi                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::constructor, url));

        func.dump ("file::ctor functor");

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("file::constructor");

        void_t ret = engine_->call <api_t, cpi_t, void_t> (cc);

        if ( cc->get_state () == Failed )
        {
          throw " file constructor failed";
        }

        return ret;
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

        func.dump ("file::get_size functor");

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("file::get_size");

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        int ret = engine_->call <api_t, cpi_t, int> (cc);

        if ( cc->get_state () == Failed )
        {
          throw " file constructor failed";
        }

        return ret;
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::task> file::get_size_async (saga::impl::call_mode::CallMode m)
      {
        typedef saga::impl::filesystem::file                                                                            api_t;
        typedef saga::impl::filesystem::file_cpi                                                                        cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, saga::util::shared_ptr <saga::impl::task>, saga::impl::call_mode::CallMode > func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::get_size_async, m));

        func.dump ("file::get_size_async functor");

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("file::get_size_async");

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        saga::util::shared_ptr <saga::impl::task> ret = engine_->call <api_t, cpi_t, saga::util::shared_ptr <saga::impl::task> > (cc);

        if ( cc->get_state () == Failed )
        {
          throw " file constructor failed";
        }

        return ret;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::copy, tgt));

        func.dump ("file::copy functor");

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("file::copy");

        void_t ret = engine_->call <api_t, cpi_t, void_t> (cc);

        if ( cc->get_state () == Failed )
        {
          throw " file constructor failed";
        }

        return ret;
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
        saga::util::scoped_lock sl (idata_->get_mutex ());
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::filesystem::dir                 api_t;
        typedef saga::impl::filesystem::dir_cpi             cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::constructor, url));

        func.dump ("dir::ctor functor");

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("dir::constructor");

        return engine_->call <api_t, cpi_t, void_t> (cc);
      }

      //////////////////////////////////////////////////////////////////
      std::string dir::get_url (void)
      {
        typedef saga::impl::filesystem::dir                       api_t;
        typedef saga::impl::filesystem::dir_cpi                   cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::get_url));

        func.dump ("dir::get_url functor");

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("dir::get_url");

        return engine_->call <api_t, cpi_t, std::string> (cc);
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::filesystem::file> dir::open (std::string url)
      {
        typedef saga::impl::filesystem::dir                       api_t;
        typedef saga::impl::filesystem::dir_cpi                   cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       saga::util::shared_ptr <saga::impl::filesystem::file>, std::string > func_t;

        saga::util::shared_ptr <func_t> func (new func_t (&cpi_t::open, url));

        func.dump ("dir::open functor");

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 

        cc->set_func_name ("dir::open");

        return engine_->call <api_t, cpi_t, saga::util::shared_ptr <saga::impl::filesystem::file> > (cc);
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga


int main ()
{
  mtrace();

  try
  {
    // file copy and get size, sync
    if ( 0 )
    {
      std::cout << " -sync file ops --------"  << std::endl;
    
      saga::filesystem::file f ("/etc/passwd");
    
      std::cout << "file size: " << f.get_size () << std::endl;
    
      f.copy ("/tmp/passwd.bak");
    
      std::cout << " -----------------------"  << std::endl;
    }

    // file open, sync
    if ( 0 )
    {
      std::cout << " -sync dir ops ---------"  << std::endl;
    
      saga::filesystem::dir d ("/etc/");
      std::cout << "dir url: " << d.get_url () << std::endl;
    
      saga::filesystem::file f = d.open ("passwd");
      std::cout << "file size: " << f.get_size () << std::endl;
    
      std::cout << " -----------------------"  << std::endl;
    }

    // file get_size, async
    if ( 1 )
    {
      std::cout << " -async file ops -------"  << std::endl;
      
      saga::filesystem::file f ("/etc/passwd");
      
      saga::task t = f.get_size <saga::impl::call_mode::Async> ();
      
      std::cout << "state: " << t.get_state () << std::endl;
      std::cout << "state: " << saga::util::saga_enums.to_key <saga::impl::call_state> (t.get_state ()) << std::endl;
      std::cout << " --- done --------------"  << std::endl;
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

  muntrace();

  return 0;
}

