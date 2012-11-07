
#include <iostream>
#include <string>

class base
{
  private:
    class idata
    {
      public:
        std::string name;

        idata (std::string name_) 
          : name (name_)
        {
        }
    };

    class impl
    {
      private:
        idata idata_;

      public: 
        impl (std::string name) 
          : idata_ (name)
        {
        }

        std::string get_name (void) 
        {
          return idata_.name;
        }
    };

  private:
    impl impl_;

  public:
    base (std::string name)
      : impl_ (name)
    { 
    }

    std::string get_name (void) 
    {
      return impl_.get_name ();
    }
};

int main ()
{
  base b ("test");
  std::cout << b.get_name () << std::endl;

  base::impl i ("impl");
  std::cout << i.get_name () << std::endl;

  return 0;
}

