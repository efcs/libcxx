#include <__tree>
#include <map>

_LIBCPP_BEGIN_NAMESPACE_STD

template struct __tree_operations<void*>;
template class __tree<string, less<string>, allocator<string> >;
template class __tree<_MapVT,
                             __map_value_compare<string, _MapVT, less<string> >,
                             allocator<_MapVT> >;

_LIBCPP_END_NAMESPACE_STD