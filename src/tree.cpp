#include <__tree>
#include <map>

_LIBCPP_BEGIN_NAMESPACE_STD

template struct __tree_operations<void*>;

_LIBCPP_TREE_INSTANT( );
_LIBCPP_MAP_INSTANTS( )

template
pair<__tree_iterator<string, __tree_node<string, void*>*, long>, bool>
__tree<string, less<string >, allocator<string > >::__emplace_unique_key_args<string, string const&>(string const&, string const&);


_LIBCPP_END_NAMESPACE_STD