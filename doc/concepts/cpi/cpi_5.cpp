
#include <mcheck.h>

#include "cpi_5.hpp"

static saga::util::shared_ptr <saga::impl::engine> the_engine (new saga::impl::engine);

  
namespace saga
{
  namespace impl
  {
    impl_base::impl_base (void)
      : engine_ (the_engine) // FIXME: no need to create a new engine - get it from the session!
    {
    }

    void impl_base::dump (std::string msg)
    {
      std::cout << "impl_base (" << this << ") : " << saga::util::demangle (typeid (*this).name ()) 
                << " : " << msg  << std::endl;
      engine_.dump ("    engine_     : ");
      engine_->dump();
    }


    task::task (saga::util::shared_ptr <saga::impl::call_context> t_cc)
      : idata_  (new task_instance_data)
    {
      SAGA_UTIL_STACKTRACE ();

      saga::util::scoped_lock sl (idata_->get_mutex ());

      idata_->t_cc = t_cc;
    }


    void_t task::constructor (void)
    {
      SAGA_UTIL_STACKTRACE ();

      saga::util::scoped_lock sl (idata_->get_mutex ());

      typedef saga::impl::void_t                           res_t;
      typedef saga::impl::task                             api_t;
      typedef saga::impl::task_cpi                         cpi_t;
      typedef saga::impl::functor_0 <api_t, cpi_t, void_t> func_t;

      saga::util::shared_ptr <func_t> func (new func_t ("constructor", &cpi_t::constructor));

      saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
      cc->init_result <res_t> ();

      engine_->call <api_t, cpi_t> (cc);

      if ( cc->get_task_state () == Failed )
      {
        throw " task::constructor () indicates failed";
      }
    }


    saga::impl::call_state task::get_state (void)
    {
      SAGA_UTIL_STACKTRACE ();

      saga::util::scoped_lock sl (idata_->get_mutex ());

      typedef saga::impl::call_state                      res_t;
      typedef saga::impl::task                            api_t;
      typedef saga::impl::task_cpi                        cpi_t;
      typedef saga::impl::functor_0 <api_t, cpi_t, res_t> func_t;

      saga::util::shared_ptr <func_t> func (new func_t ("get_state", &cpi_t::get_state));

      saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
      cc->init_result <res_t> ();

      engine_->call <api_t, cpi_t> (cc);

      if ( cc->get_state () == Failed )
      {
        throw " task::get_state failed - can't get result";
      }

      return cc->get_result <res_t> ();
    }

    saga::util::shared_ptr <result_t> task::get_result (void)
    {
      SAGA_UTIL_STACKTRACE ();

      saga::util::scoped_lock sl (idata_->get_mutex ());

      typedef saga::util::shared_ptr <result_t>            res_t;
      typedef saga::impl::task                             api_t;
      typedef saga::impl::task_cpi                         cpi_t;
      typedef saga::impl::functor_0 <api_t, cpi_t, res_t > func_t;

      saga::util::shared_ptr <func_t> func (new func_t ("get_result", &cpi_t::get_result));

      saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
      cc->init_result <res_t> ();

      engine_->call <api_t, cpi_t> (cc);

      if ( cc->get_task_state () == Failed )
      {
        throw " task::get_task_state indicates failed";
      }

      return cc->get_result <res_t> ();
    }

    namespace filesystem
    {
      //////////////////////////////////////////////////////////////////
      file::file (void)                    // FIXME: the file impl and pimpl object hierarchies are ignored
        : idata_ (new file_instance_data)
      {
        SAGA_UTIL_STACKTRACE ();

        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());

        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::void_t                                        res_t;
        typedef saga::impl::filesystem::file                              api_t;
        typedef saga::impl::filesystem::file_cpi                          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor", &cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " file::constructor indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      int file::get_size (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef int                                       res_t;
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, int> func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t ("get_size", &cpi_t::get_size));

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " file::get_size indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::task> file::get_size (saga::impl::call_mode m)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::task>                                                               res_t;
        typedef saga::impl::filesystem::file                                                                            api_t;
        typedef saga::impl::filesystem::file_cpi                                                                        cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, saga::util::shared_ptr <saga::impl::task>, saga::impl::call_mode> func_t;

        // create a functor which hold the cpi class' get_size() function
        // pointer.  The second templ parameter is the functions return type
        saga::util::shared_ptr <func_t> func (new func_t ("get_size", &cpi_t::get_size, m));

        // create a call context wich holds functor and implementation
        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        // the cc is given to the engine, so it can use the functor to call that
        // function on some cpi
        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " file::get_size <> () indicates failed";
        }

        res_t ret = cc->get_result <res_t> ();

        ret.dump ();
        cc.dump ();
        cc->dump ();

        return ret;
      }

      //////////////////////////////////////////////////////////////////
      void_t file::copy (std::string tgt)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::impl::void_t                        res_t;
        typedef saga::impl::filesystem::file              api_t;
        typedef saga::impl::filesystem::file_cpi          cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, void_t, std::string>    func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("copy", &cpi_t::copy, tgt));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " file::copy () indicates failed";
        }

        return cc->get_result <res_t> ();
      }




      //////////////////////////////////////////////////////////////////
      dir::dir (void) // FIXME: the dir impl and pimpl object hierarchies are ignored
        : idata_ (new dir_instance_data)
      {
        SAGA_UTIL_STACKTRACE ();

        idata_->valid = false;
        idata_->url   = "";
        idata_->pos   = 0;
      }

      //////////////////////////////////////////////////////////////////
      void_t dir::constructor (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        saga::util::scoped_lock sl (idata_->get_mutex ());
        idata_->url   = url;
        idata_->pos   = 0;
        idata_->valid = true;

        typedef saga::impl::void_t                          res_t;
        typedef saga::impl::filesystem::dir                 api_t;
        typedef saga::impl::filesystem::dir_cpi             cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       void_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("constructor",&cpi_t::constructor, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);
        
        if ( cc->get_task_state () == Failed )
        {
          throw " dir::constructor () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      std::string dir::get_url (void)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef std::string                                       res_t;
        typedef saga::impl::filesystem::dir                       api_t;
        typedef saga::impl::filesystem::dir_cpi                   cpi_t;
        typedef saga::impl::functor_0 <api_t, cpi_t, std::string> func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("get_url", &cpi_t::get_url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " dir::get_url () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

      //////////////////////////////////////////////////////////////////
      saga::util::shared_ptr <saga::impl::filesystem::file> dir::open (std::string url)
      {
        SAGA_UTIL_STACKTRACE ();

        typedef saga::util::shared_ptr <saga::impl::filesystem::file> res_t;
        typedef saga::impl::filesystem::dir                           api_t;
        typedef saga::impl::filesystem::dir_cpi                       cpi_t;
        typedef saga::impl::functor_1 <api_t, cpi_t, 
                                       saga::util::shared_ptr <saga::impl::filesystem::file>, std::string > func_t;

        saga::util::shared_ptr <func_t> func (new func_t ("open", &cpi_t::open, url));

        saga::util::shared_ptr <saga::impl::call_context> cc (new saga::impl::call_context (func, shared_this <api_t> ())); 
        cc->init_result <res_t> ();

        engine_->call <api_t, cpi_t> (cc);

        if ( cc->get_task_state () == Failed )
        {
          throw " dir::open () indicates failed";
        }

        return cc->get_result <res_t> ();
      }

    } // namespace filesystem

  } // namespace impl

} // namespace saga


int main ()
{
  saga::util::stack_tracer::enabled = false;

  SAGA_UTIL_STACKTRACE ();

  try
  {
    // file copy and get size, sync
    if ( 0 )
    {
      std::cout << " -sync file ops ########"  << std::endl;
    
      saga::filesystem::file f ("/etc/passwd");
    
      std::cout << "file size: " << f.get_size () << std::endl;
    
      f.copy ("/tmp/passwd.bak");
    
      std::cout << " #######################"  << std::endl;
    }

    // file open, sync
    if ( 0 )
    {
      std::cout << " -sync dir ops #########"  << std::endl;
    
      saga::filesystem::dir d ("/etc/");
      std::cout << "dir url: " << d.get_url () << std::endl;
    
      saga::filesystem::file f = d.open ("passwd");
      std::cout << "file size: " << f.get_size () << std::endl;
    
      std::cout << " #######################"  << std::endl;
    }

    // file get_size, async
    if ( 1 )
    {
      std::cout << " -async file ops #######"  << std::endl;
      
      saga::filesystem::file f ("/etc/passwd");

      std::cout << " 1 #####################"  << std::endl;
      
      // std::cout << "file size: " << f.get_size () << std::endl;

      saga::task t = f.get_size <saga::impl::Async> ();
      
      std::cout << " 2 #####################"  << std::endl;

      saga::impl::call_state s = t.get_state ();

      while ( s == saga::impl::New     ||
              s == saga::impl::Running )
      {
        std::cout << "state: " << saga::util::saga_enums.to_key <saga::impl::call_state> (t.get_state ()) << std::endl;
        ::sleep (1);
        s = t.get_state ();
      }

      std::cout << "state: " << saga::util::saga_enums.to_key <saga::impl::call_state> (t.get_state ()) << std::endl;
      std::cout << " 3 #####################"  << std::endl;

      std::cout << "value: " << t.get_result <int> () << std::endl;

      std::cout << " ### done ##############"  << std::endl;

      ::sleep (2);
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

