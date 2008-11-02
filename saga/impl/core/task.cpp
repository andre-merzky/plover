
#include <set>
#include <string>

#include <saga/impl/core/task.hpp>
#include <saga/impl/core/object.hpp>

#include <saga/saga/core/task.hpp>

saga::impl::task::task  (void)
  : object      (saga::object::Task),
    monitorable ()
{
}

