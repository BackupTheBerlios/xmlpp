#ifndef ITERATORS_HPP
#define ITERATORS_HPP

#include <tinyxml.h>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>

namespace xmlpp
{
    /**
     * Pattern for element iterators
     */
    template<class T>
    class element_iterator_impl :
        public boost::iterator_facade<
            element_iterator_impl<T>,
            T,
            boost::bidirectional_traversal_tag
        >
    {
    private:
        typedef typename boost::remove_const<T>::type element_t;
        friend class boost::iterator_core_access;

    private:
        element_t elem;

    private:
        void increment()
        {
            elem = elem.NextSiblingElement();
        }

        bool equal(element_iterator_impl const& other) const
        {
            return elem == other.elem;
        }
        T& dereference() const { return *const_cast<element_t*>(&elem); }

    public:
        // Construct
        element_iterator_impl(const element_iterator_impl& rhs) : elem(rhs.elem) {}
        element_iterator_impl(const T& elem) : elem(elem) {}
        template<class P>
        element_iterator_impl(const element_iterator_impl<P>& rhs) : elem(*rhs) {}
        explicit element_iterator_impl(TiXmlElement* elem) : elem(elem) {}
        //explicit element_iterator_impl(TiXmlElement const* elem) : elem(elem) {}
        //template<class T>
        //explicit element_iterator_impl(const T& rhs) : elem(rhs) {}

        bool operator !() const { return elem.GetTiXmlNode() == NULL; }
    };

    /**
     * Pattern for node iterators
     */
    template<class T>
    class node_iterator_impl :
        public boost::iterator_facade<
            node_iterator_impl<T>,
            T,
            boost::bidirectional_traversal_tag
        >
    {
    private:
        typedef typename boost::remove_const<T>::type node_t;
        friend class boost::iterator_core_access;

    private:
        node_t node;

    private:
        void increment()
        {
            node = node.NextSibling();
        }
        void decrement()
        {
            node = node.PreviousSibling();
        }

        bool equal(node_iterator_impl const& other) const
        {
            return node == other.node;
        }
        T& dereference() const { return *const_cast<node_t*>(&node); }

    public:
        // Construct
        node_iterator_impl(const node_iterator_impl& rhs) : node(rhs.node) {}
        node_iterator_impl(const T& node) : node(node) {}
        template<class P>
        node_iterator_impl(const element_iterator_impl<P>& rhs) : node( node_t( rhs->GetTiXmlNode() ) ) {}
        explicit node_iterator_impl(TiXmlNode* p) : node(p) {}
        //explicit node_iterator_impl(TiXmlNode const* elem) : node(node) {}

        bool operator !() const { return node.GetTiXmlNode() == NULL; }
    };
}

#endif // ITERATORS_HPP
