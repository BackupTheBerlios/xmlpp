#ifndef XMLPP_ELEMENT_H
#define XMLPP_ELEMENT_H

#include "node.h"
#include "attribute.h"

namespace xmlpp {

/**
 * Class represents xml element
 */
class element :
    public node_impl<TiXmlElement>
{
private:
    /** Pattern for attribute iterators
     */
    template<typename T>
    class attribute_iterator_impl :
        public boost::iterator_facade<
            attribute_iterator_impl<T>,
            T,
            boost::bidirectional_traversal_tag
        >
    {
    friend class boost::iterator_core_access;
    private:
        typedef typename boost::remove_const<T>::type node_type;

    private:
        void increment()
        {
            node = T( node.tixmlAttribute->Next() != NULL
                      ? node.tixmlAttribute->Next()
                       : NULL );
        }

        void decrement()
        {
            node = T( node.tixmlAttribute->Previous() != NULL
                      ? node.tixmlAttribute->Previous()
                      : NULL );
        }

        bool equal(attribute_iterator_impl const& other) const
        {
            return node == other.node;
        }

        T& dereference() const { return const_cast<node_type&>(node); }

    public:
        // Construct
        attribute_iterator_impl() : node(NULL) {}
        attribute_iterator_impl(const attribute_iterator_impl& rhs) : node(rhs.node) {}
        explicit attribute_iterator_impl(const T& attribute) : node(attribute) {}
        explicit attribute_iterator_impl(TiXmlAttribute* pAttr) : node(pAttr) {}

        operator bool () const { return node != NULL; }

    private:
        node_type node;
    };

public:
    /// iterator to iterate throught attributes
    typedef attribute_iterator_impl<attribute>          attribute_iterator;
    /// const iterator to iterate throught attributes
    typedef attribute_iterator_impl<attribute const>    const_attribute_iterator;

public:
    // Construct
    element();
    element(const element& rhs);
    explicit element(TiXmlElement* _tixmlElement);

    /** Create element with specified value */
    explicit element(const char* value);

    /** Create element with specified value */
    explicit element(const std::string& value);

    /** Try interpret node as element. Could throw dom_error */
    explicit element(const node& elementNode);

    /** Get tinyxml element of this element */
    TiXmlElement* get_tixml_element() { return query_node(); }

    /** Get tinyxml element of this element */
    const TiXmlElement* get_tixml_element() const { return query_node(); }

    /** Set tiny xml element. Use with care 
     * @param _tixmlNode - new tinyxml node for this node
     */
    void set_tixml_element(TiXmlElement* _tixmlElement) { tixmlNode = _tixmlElement; }

    /** Text of the element( Same as TiXmlElement::GetText() )
     * @return text of the element
     */
    const char* get_text() const;

    /** Get attribute value by the name
     * @param output value
     * @return attribute value
     */
    template<class T>
    T& get_attribute_value(const char* name, T& value) const
    {
        if ( has_attribute(name) )
        {
            std::istringstream ss( get_attribute(name) );
            ss >> value;

            if ( ss.fail() ) {
                throw dom_error("Wrong attribute type");
            }

            return value;
        }

        throw dom_error("Attribute not found");
    }

	template<typename T>
	void set_attribute_value(const char* name, const T& value)
	{
		std::ostringstream ss;
		ss << value;
		set_attribute( name, ss.str() );
	}

    /** Get attribute value by the name
     * @return attribute value
     */
    template<class T>
    T get_attribute_value(const char* name) const
    {
        T value;
        return get_attribute_value(name, value);
    }

    /**
     * Set text of the element.
     * For example: There is <b></b> tag. After SetText("txt") it became <b>txt</b>.
     * Example2: <b><d/></b> -> <b>txt<d/></b>
     * Example3: <b><d/>txt2</b> -> <b>txt<d/></b>
     * @param text of the element
     */
    void set_text(const char* text);

    /**
     * Add child to the element.
     */
    node_iterator add_child(node& n);

    /**
     * Add child to the element after specified child.
     * @param where - child afther which to insert node.
     * @param n - node to insert.
     */
    node_iterator insert_after_child(const node_iterator& where, node& n);

    /**
     * Add child to the element after specified child.
     * @param where - child afther which to insert node.
     * @param n - node to insert.
     */
    node_iterator insert_before_child(const node_iterator& where, node& n);

    /**
     * Move to next sibling
     * @return element addressing next sibling
     */
    element_iterator next_sibling_element() const;

    /** 
     * Set attribute to a given value. Attribute will be created if doesn't exist,
     * or changed if it does.
     * @param name - name of the attribute.
     * @param text - text of the attribute.
     */
    void set_attribute(const char* name, const std::string& text);

    /**
     * Check if element has attribute with specified name
     * @param name - name of the attribute
     * @return true if has
     */
    bool has_attribute(const char* name) const;

    /** Get attribute value by the name
     * @return value of the attribute with specified name
     * @throws dom_error if attribute not found.
     */
    const char* get_attribute(const char* name) const;

    /** Read attribute value by name.
     * @param name - name of the attribute.
     * @param string - output string, if NULL then function returns the length of the attribute string.
     * @param maxSize - max length of the string to store.
     * @return number of stored symbols.
     * @throws dom_error if attribute not found.
     */
    size_t read_attribute(const char* name, 
                          char*       string, 
                          size_t      maxSize) const;

    /** Get iterator addressing first attribute with specified name
     * @param name - name of the attribute
     */
    attribute_iterator first_attribute(const char* name);

    /** Get iterator addressing first attribute with specified name
     * @param name - name of the attribute
     */
    const_attribute_iterator first_attribute(const char* name) const;

    /** Get iterator addressing first attribute
     * @return iterator addressing first attribute
     */
    attribute_iterator first_attribute();

    /** Get iterator addressing first attribute
     * @return const iterator addressing first attribute
     */
    const_attribute_iterator first_attribute() const;

    /** Get end attribute iterator
     * @return iterator addressing attribute after last attribute
     */
    attribute_iterator end_attribute();

    /** Get end attribute iterator
     * @return const iterator addressing attribute after last attribute
     */
    const_attribute_iterator end_attribute() const;
};

/** Extract attribute value from the element.
 * @param name - name of the attribute.
 * @param elem - element with attribute.
 * @param defaultValue - default value for the attribute if it wasn't found.
 */
template<typename T>
T read_attribute(const char*    name, 
                 const element& elem, 
                 T              defaultValue)
{
    if ( elem.has_attribute(name) )
    {
        T value;
        std::istringstream ss( elem.get_attribute(name) );
        ss >> value;

        if ( ss.fail() ) {
            return defaultValue;
        }

        return value;
    }

    return defaultValue;
}

/** Extract attribute value from the element. Throws if can't read attribute.
 * @param name - name of the attribute.
 * @param elem - element with attribute.
 * @return extracted attribute.
 */
template<typename T>
T read_attribute(const char*    name, 
                 const element& elem)
{
    if ( elem.has_attribute(name) )
    {
        T value;
        std::istringstream ss( elem.get_attribute(name) );
        ss >> value;

        if ( ss.fail() ) {
            throw dom_error("Wrong attribute type");
        }

        return value;
    }

    throw dom_error("Attribute not found");
}

} // namespace xmlpp

#endif // XMLPP_ELEMENT_H
