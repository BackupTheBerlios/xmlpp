#ifndef XMLPP_ITERATORS_HPP
#define XMLPP_ITERATORS_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace xmlpp {

class node;
class element;

template<typename T>
struct node_iterator_impl_traits;

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
friend class boost::iterator_core_access;
private:
    typedef node_iterator_impl_traits<T>            impl_traits;
    typedef typename impl_traits::node_type         node_type;
    typedef typename impl_traits::internal_pointer  internal_pointer;

private:
    void increment();
    void decrement();

    bool equal(node_iterator_impl const& other) const { return node == other.node; }
    T&   dereference() const                          { return const_cast<node_type&>(node); }

public:
    /// construct end iterator
    node_iterator_impl() {}
    
    /// construct from xmlpp::node_iterator
    node_iterator_impl(const node_iterator_impl& rhs) : 
        node(rhs.node) {}
    
    /// Construct from xmlpp::node
    explicit node_iterator_impl(T node_) : 
        node(node_) {}

    /// Construct from tixml node
    explicit node_iterator_impl(internal_pointer node_) : 
        node(node_) {}

    operator bool () const { return node.get_tixml_element() != NULL; }

private:
    node_type node;
};

} // namespace xmlpp

#endif // XMLPP_ITERATORS_HPP
