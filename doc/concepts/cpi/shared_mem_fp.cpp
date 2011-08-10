
#include <iostream>

#include <saga/util/shared_ptr.hpp>

class A
{
  public:
    int inc (int x) { std::cout << "A::inc (" << x << ")" << std::endl; return (x+1); }
    int dec (int x) { std::cout << "A::dec (" << x << ")" << std::endl; return (x-1); }
};

typedef int (A::*fp)(int x);
#define CALL_FP(obj,fp)  ((obj).*(fp))
#define CALL_FPP(obj,fp)  ((*obj).*(fp))

int main ()
{
  fp inc_fp = &A::inc;
  fp dec_fp = &A::dec;

  A a1;

  CALL_FP (a1, inc_fp)(3);
  CALL_FP (a1, dec_fp)(3);

  A* a2 = new A;

  CALL_FP (*a2, inc_fp)(4);
  CALL_FP (*a2, dec_fp)(4);

  CALL_FPP (a2, inc_fp)(5);
  CALL_FPP (a2, dec_fp)(5);

  saga::util::shared_ptr <A> a3 (new A);

  CALL_FPP (a3, inc_fp)(6);
  CALL_FPP (a3, dec_fp)(6);

  return 0;
};

