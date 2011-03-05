
#ifndef SAGA_UTIL_SHAREDPTR_HPP
#define SAGA_UTIL_SHAREDPTR_HPP


// we are able to use tr1 shared pointers (wrapped and decorated with some added
// convenience calls), or use our own simple shared_ptr implementation

#ifdef SAGA_USE_TR1 
# include <saga/util/shared_ptr_tr1.hpp>
#else
# include <saga/util/shared_ptr_saga.hpp>
#endif


#endif // SAGA_UTIL_SHAREDPTR_HPP

