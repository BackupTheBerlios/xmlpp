#include "iterator.h"
#include "tinyxml.h"

namespace xmlpp {

template<>
void node_iterator_impl<node>::increment()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->NextSibling() );
}

template<>
void node_iterator_impl<node>::decrement()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->PreviousSibling() );
}

template<>
void node_iterator_impl<const node>::increment()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->NextSibling() );
}

template<>
void node_iterator_impl<const node>::decrement()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->PreviousSibling() );
}

template<>
void node_iterator_impl<element>::increment()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->NextSiblingElement() );
}

template<>
void node_iterator_impl<element>::decrement()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->PreviousSiblingElement() );
}

template<>
void node_iterator_impl<const element>::increment()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->NextSiblingElement() );
}

template<>
void node_iterator_impl<const element>::decrement()
{
    assert( node.get_tixml_node() );
    node = node_type( node.get_tixml_node()->PreviousSiblingElement() );
}

} // namespace xmlpp