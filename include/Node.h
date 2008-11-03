#ifndef NODE_H
#define NODE_H

#include "Iterators.hpp"
#include <stdexcept>
#include <boost/static_assert.hpp>

namespace xmlpp
{
    /**
     * Exception occured during parsing xml file
     */
    class DOMException :
        public std::runtime_error
    {
    public:
        DOMException() : std::runtime_error("XML error") {}
        DOMException(const DOMException& rhs) : std::runtime_error(rhs) {}
        explicit DOMException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class Node;
    class Element;

    /// Iterator type for iterating throught xml nodes
    typedef node_iterator_impl<Node>                node_iterator;
    /// Iterator type for iterating throught xml nodes
    typedef node_iterator_impl<Node const>          const_node_iterator;
    /// Const iterator type for iterating throught xml elements
    typedef element_iterator_impl<Element>          element_iterator;
    /// Const iterator type for iterating throught xml elements
    typedef element_iterator_impl<Element const>    const_element_iterator;

    /**
     * Node implementation class wraps around TiXml classes
     */
    class Node
    {
    private:
        friend class Document;

    protected:
        TiXmlNode*  pNode;

    public:
        // Construct
        Node() : pNode(NULL) {}
        Node(const Node& rhs) : pNode(rhs.pNode) {}
        explicit Node(TiXmlNode* p) : pNode(p) {}

        /**
         * Get TiXmlNode
         * @return TiXmlNode it wraps
         */
        TiXmlNode* ToTiXmlNode() const;

        /**
         * Value of the xml element
         * @return value of the xml element
         */
        std::string Value() const;

        /**
         * Setup new value to the element
         * @param new value
         */
        void SetValue(const std::string& newVal);

        /** One step up the DOM
         * @return iterator addresing parent element
         */
        const_element_iterator Parent() const;

        /** One step up the DOM
         * @return iterator addresing parent element
         */
        element_iterator Parent();

        /** Move to next sibling
         * @return node addressing next sibling
         */
        Node NextSibling() const;

        /** Move to previous sibling
         * @return node addressing previous sibling
         */
        Node PreviousSibling() const;

        /** Clear all the children of this node */
        void Clear();

        /** Get iterator to the first child node
         * @return iterator addressing first child node
         */
        node_iterator FirstChild();

        /** Get const iterator to the first child node
         * @return const iterator addressing first child node
         */
        const_node_iterator FirstChild() const;

        /** Get iterator to the node after last node
         * @return iterator addressing node after last node
         */
        node_iterator EndChild();

        /** Get const iterator to the node after last node
         * @return const iterator addressing node after last node
         */
        const_node_iterator EndChild() const;

        /** Get iterator to the first child node
         * @return iterator addressing first child node
         */
        element_iterator FirstChildElement(const char* value = NULL);

        /** Get const iterator to the first child element
         * @return const iterator addressing first child element
         */
        const_element_iterator FirstChildElement(const char* value = NULL) const;

        /** Get iterator to the node after last element
         * @return iterator addressing node after last element
         */
        element_iterator EndChildElement();

        /** Get const iterator to the node after last element
         * @return const iterator addressing node after last element
         */
        const_element_iterator EndChildElement() const;

        /** Compare nodes */
        bool operator == (const Node& rhs) const;
    };

    /**
     * NodeImpl class converts TiXmlNode to specified class(e.g. TiXmlElement)
     */
    template<class T>
    class NodeImpl :
        public Node
    {
    protected:
        NodeImpl() {}
        NodeImpl(T* p) : Node(p) {}
        NodeImpl(const NodeImpl& rhs) : Node(rhs) {}

        T* QueryNode() { return dynamic_cast<T*>(pNode); }
        T const* QueryNode() const { return dynamic_cast<T const*>(pNode); }

    public:
        bool Valid() const { return pNode != 0; }
    };
}

#endif // NODE_H
