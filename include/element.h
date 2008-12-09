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
    template<class attribute_t>
    class attribute_iterator_impl :
        public boost::iterator_facade<
            attribute_iterator_impl<attribute_t>,
            attribute_t,
            boost::bidirectional_traversal_tag
        >
    {
    private:
        // Instantiate only from Attribute class
        //BOOST_STATIC_ASSERT( (boost::is_convertible<attribute_t&, Attribute const&>::value_type) );

    private:
        attribute_t node;

    private:
        friend class boost::iterator_core_access;

        void increment()
        {
            node = attribute_t( node.pAttribute->Next() != NULL
                                ? node.pAttribute->Next()
                                : NULL );
        }
        void decrement()
        {
            node = attribute_t( node.pAttribute->Previous() != NULL
                                ? node.pAttribute->Previous()
                                : NULL );
        }

        bool equal(attribute_iterator_impl const& other) const
        {
            return node == other.node;
        }

        attribute_t& dereference() const { return *node; }

    public:
        // Construct
        attribute_iterator_impl() : node(NULL) {}
        attribute_iterator_impl(const attribute_iterator_impl& rhs) : node(rhs.node) {}
        explicit attribute_iterator_impl(const attribute_t& attribute) : node(attribute) {}
        explicit attribute_iterator_impl(TiXmlAttribute* pAttr) : node(pAttr) {}

        bool operator !() const { return node == NULL; }
    };

public:
    /// iterator to iterate throught attributes
    typedef attribute_iterator_impl<attribute>          attribute_iterator;
    /// const iterator to iterate throught attributes
    typedef attribute_iterator_impl<attribute const>    const_attribute_iterator;

public:
    // Construct
    element() :
        node_impl<TiXmlElement>(NULL) {}
    element(const element& rhs) :
        node_impl<TiXmlElement>(rhs) {}
    explicit element(const node& elementNode) :
        node_impl<TiXmlElement>( const_cast<node&>(elementNode).get_tixml_node()->ToElement() ) {}
    explicit element(TiXmlElement* _tixmlElement) :
        node_impl<TiXmlElement>(_tixmlElement) { assert(_tixmlElement); }
    explicit element(const std::string& value) :
        node_impl<TiXmlElement>( new TiXmlElement(value) ) {}

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
    std::string get_text() const;

    /**
     * Set text of the element.
     * For example: There is <b></b> tag. After SetText("txt") it became <b>txt</b>.
     * Example2: <b><d/></b> -> <b>txt<d/></b>
     * Example3: <b><d/>txt2</b> -> <b>txt<d/></b>
     * @param text of the element
     */
    void set_text(const std::string text);

    /**
     * Move to next sibling
     * @return element addressing next sibling
     */
    element_iterator next_sibling_element() const;

    /**
     * Check if element has attribute with specified name
     * @param name - name of the attribute
     * @return true if has
     */
    bool has_attribute(const std::string& name) const;

    /** Get attribute value by the name
     * @return value of the attribute with specified name
     */
    std::string get_attribute(const std::string& name) const;

    /** Get attribute value by the name
     * @param output value
     * @return attribute value
     */
    template<class T>
    T& get_attribute_value(const std::string& name, T& value) const
    {
        int res = QueryNode()->QueryValueAttribute(name, &value);
        if ( res == TIXML_WRONG_TYPE ) {
            throw dom_error("Wrong attribute type");
        }
        else if ( res == TIXML_NO_ATTRIBUTE ) {
            throw dom_error("Attribute not found");
        }
        return value;
    }

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

} // namespace xmlpp

#endif // XMLPP_ELEMENT_H
