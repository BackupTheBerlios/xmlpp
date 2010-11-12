#ifndef XMLPP_NODE_H
#define XMLPP_NODE_H

#include "iterators.hpp"
#include <stdexcept>

namespace xmlpp {

/**
 * Exception occured during parsing xml file
 */
class dom_error :
    public std::runtime_error
{
public:
    dom_error() : 
        std::runtime_error("XML format error") {}
    dom_error(const dom_error& rhs) : 
        std::runtime_error(rhs) {}
    explicit dom_error(const std::string& msg) : 
        std::runtime_error(msg) {}
};

class node;
class element;

/// Iterator type for iterating throught xml nodes
typedef node_iterator_impl<node>                node_iterator;
/// Iterator type for iterating throught xml nodes
typedef node_iterator_impl<node const>          const_node_iterator;
/// Const iterator type for iterating throught xml elements
typedef element_iterator_impl<element>          element_iterator;
/// Const iterator type for iterating throught xml elements
typedef element_iterator_impl<element const>    const_element_iterator;

/**
 * Node implementation class wraps around TiXml classes
 */
class node
{
public:
    /** Construct empty node(tixmlNode == NULL).
     * TODO: Think about design. May be it is better to construct empty but existing TiXmlNode
     */
    node();
    node(const node& rhs);
    explicit node(TiXmlNode* _tixmlNode);

    /** Get TiXmlNode
     * @return TiXmlNode it wraps
     */
    TiXmlNode* get_tixml_node() { return tixmlNode; }

    /** Get TiXmlNode
     * @return TiXmlNode it wraps
     */
    const TiXmlNode* get_tixml_node() const { return tixmlNode; }

    /** Set tiny xml node. Use with care 
     * @param _tixmlNode - new tinyxml node for this node
     */
    void set_tixml_node(TiXmlNode* _tixmlNode) { tixmlNode = _tixmlNode; }

    /** Get value of the xml element
     * @return value of the xml element
     */
    std::string get_value() const;

    /** Setup new value to the element
     * @param new value
     */
    void set_value(const std::string& newVal);

    /** One step up the DOM
     * @return iterator addresing parent element
     */
    const_element_iterator get_parent() const;

    /** One step up the DOM
     * @return iterator addresing parent element
     */
    element_iterator get_parent();

    /** Get iterator to the first child node
     * @return iterator addressing first child node
     */
    node_iterator first_child();

    /** Get const iterator to the first child node
     * @return const iterator addressing first child node
     */
    const_node_iterator first_child() const;

    /** Get iterator to the node after last node
     * @return iterator addressing node after last node
     */
    node_iterator end_child();

    /** Get const iterator to the node after last node
     * @return const iterator addressing node after last node
     */
    const_node_iterator end_child() const;

    /** Get iterator to the first child element
     * @return iterator addressing first child node
     */
    element_iterator first_child_element();

    /** Get const iterator to the first child element
     * @return const iterator addressing first child element
     */
    const_element_iterator first_child_element() const;

    /** Get iterator to the first child node with specified name
     * @return iterator addressing first child node
     */
    element_iterator first_child_element(const std::string& value);

    /** Get const iterator to the first child element with specified name
     * @return const iterator addressing first child element
     */
    const_element_iterator first_child_element(const std::string& value) const;

    /** Get iterator to the node after last element
     * @return iterator addressing node after last element
     */
    element_iterator end_child_element();

    /** Get const iterator to the node after last element
     * @return const iterator addressing node after last element
     */
    const_element_iterator end_child_element() const;

	/** Remove all child nodes */
	void clear();

    /** Compare nodes */
    bool operator == (const node& rhs) const;

protected:
    TiXmlNode* tixmlNode;
};

/**
 * NodeImpl class converts TiXmlNode to specified class(e.g. TiXmlElement)
 */
template<class T>
class node_impl :
    public node
{
public:
    typedef T tixml_node_type;

protected:
    node_impl() 
        {}
    node_impl(T* p) : 
        node(p) {}
    node_impl(const node_impl& rhs) : 
        node(rhs) {}

    T* query_node() { return dynamic_cast<T*>(tixmlNode); }
    T const* query_node() const { return dynamic_cast<T const*>(tixmlNode); }
};

/** Replace specified node with new node. Could throw dom_error.
 * @param what - node for replacement
 * @param with - new node to replace old node
 */
void replace_node( node& what,
                   node& with );

/** insert node before specified node. Could throw dom_error.
 * @param beforeThis - before this node will be inserted node
 * @param node - node to insert
 */
void insert_before_node( node& beforeThis,
                         node& insertNode );

/** Insert node after specified node. Could throw dom_error.
 * @param afterThis - after this node will be inserted node
 * @param node - node to insert
 */
void insert_after_node( node& afterThis,
                        node& insertNode );

/** Make node child of another node. Could throw dom_error.
 * @param parent - parent node
 * @param child - new child node
 */
void add_child( node& parent,
                node& child );

/** Remove specified node. Could throw dom_error.
 * @param what - node to remove
 */
void remove_node( node& what );

} // namespace xmlpp

#endif // XMLPP_NODE_H
