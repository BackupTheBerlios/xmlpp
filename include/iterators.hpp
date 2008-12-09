#ifndef XMLPP_ITERATORS_HPP
#define XMLPP_ITERATORS_HPP

#include "tinyxml.h"
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace xmlpp {

/**
 * Pattern for element iterators
 */
template<typename T>
class element_iterator_impl :
    public boost::iterator_facade<
        element_iterator_impl<T>,
        T,
        boost::bidirectional_traversal_tag
    >
{
private:
    typedef typename boost::remove_const<T>::type element_type;
    friend class boost::iterator_core_access;

private:
    element_type element;

private:
    void increment()
    {
        assert( element.get_tixml_element() );
        element = element_type( element.get_tixml_element()->NextSiblingElement() );
    }
    void decrement()
    {
        assert( element.get_tixml_element() );
        element = element_type( element.get_tixml_element()->PreviousSiblingElement() );
    }

    bool equal(element_iterator_impl const& other) const
    {
        return element == other.element;
    }

    T& dereference() const 
    { 
        assert( element.get_tixml_element() );
        return const_cast<element_type&>(element); 
    }

public:
    /// construct end iterator
    element_iterator_impl() {}
              
    /// construct from xmlpp::element_iterator
    template<class P>
    element_iterator_impl(const element_iterator_impl<P>& rhs) : 
        element( const_cast<TiXmlElement*>(rhs->get_tixml_element()) ) {}

    /// construct from xmlpp::element
    element_iterator_impl(T& _element) : 
        element(_element) {}

    /// construct from tinyxml element
    explicit element_iterator_impl(TiXmlElement* _tixmlElement) : 
        element(_tixmlElement) {}

    bool operator !() const { return element.get_tixml_element() == NULL; }

	bool exist() const { return element.get_tixml_element() != NULL; }
};

/**
 * Pattern for node iterators
 */
template<typename T>
class node_iterator_impl :
    public boost::iterator_facade<
        node_iterator_impl<T>,
        T,
        boost::bidirectional_traversal_tag
    >
{
private:
    typedef typename boost::remove_const<T>::type node_type;
    friend class boost::iterator_core_access;

private:
    node_type node;

private:
    void increment()
    {
        assert( node.get_tixml_node() );
        node = node_type( node.get_tixml_node()->NextSibling() );
    }
    void decrement()
    {
        assert( node.get_tixml_node() );
        node = node_type( node.get_tixml_node()->PreviousSibling() );
    }

    bool equal(node_iterator_impl const& other) const
    {
        return node == other.node;
    }

    T& dereference() const 
    { 
        assert( node.get_tixml_node() ); 
        return const_cast<node_type&>(node); 
    }

public:
    /// construct end iterator
    node_iterator_impl() {}
    
    /// construct from xmlpp::node_iterator
    node_iterator_impl(const node_iterator_impl& rhs) : 
        node(rhs.node) {}

    /// construct from xmlpp::element_iterator
    template<class P>
    node_iterator_impl(const element_iterator_impl<P>& rhs) : 
        node( rhs.get_tixml_node() ) {}

    /// construct from xmlpp::node
    explicit node_iterator_impl(T& _node) : 
        node(_node) {}

    /// Construct from tixml node
    explicit node_iterator_impl(TiXmlNode* _tixmlNode) : 
        node(_tixmlNode) {}

    bool operator !() const { return node.get_tixml_node() == NULL; }

	bool exist() const { return node.get_tixml_node() != NULL; }
};

} // namespace xmlpp

#endif // XMLPP_ITERATORS_HPP
