#include <__tree>

_LIBCPP_BEGIN_NAMESPACE_STD

template struct __tree_operations<void*>;
template class __tree<string, less<string>, allocator<string> >;

_LIBCPP_END_NAMESPACE_STD