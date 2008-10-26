#ifndef ELEMENT_H
#define ELEMENT_H

#include "Node.h"
#include "Attribute.h"

namespace xmlpp
{
    /**
     * Class represents xml element
     */
    class Element :
        public NodeImpl<TiXmlElement>
    {
    private:
        /**
         * Pattern for attribute iterators
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
        typedef attribute_iterator_impl<Attribute>          attribute_iterator;
        /// const iterator to iterate throught attributes
        typedef attribute_iterator_impl<Attribute const>    const_attribute_iterator;

    public:
        // Construct
        Element() :
            NodeImpl<TiXmlElement>(NULL) {}
        Element(const Element& rhs) :
            NodeImpl<TiXmlElement>(rhs) {}
        explicit Element(const Node& node) :
            NodeImpl<TiXmlElement>( node.ToTiXmlNode()->ToElement() ) {}
        explicit Element(TiXmlElement* pElem) :
            NodeImpl<TiXmlElement>(pElem) {}
        explicit Element(const std::string& value) :
            NodeImpl<TiXmlElement>( new TiXmlElement(value) ) {}

        /**
         * Text of the element( Same as TiXmlElement::GetText() )
         * @return text of the element
         */
        std::string Text() const;

        /**
         * Set text of the element.
         * For example: There is <b></b> tag. After SetText("txt") it became <b>txt</b>.
         * Example2: <b><d/></b> -> <b>txt<d/></b>
         * Example3: <b><d/>txt2</b> -> <b>txt<d/></b>
         * @param text of the element
         */
        void SetText(const std::string text);

        /**
         * Move to next sibling
         * @return element addressing next sibling
         */
        Element NextSiblingElement() const;

        /**
         * Check if element has attribute with specified name
         * @param name of the attribute
         * @return true if has
         */
        bool HasAttribute(const std::string& name) const;

        /**
         * Get attribute by the name
         * @return value of the attribute with specified name
         */
        std::string Attr(const std::string& name) const;

        /**
         * Get attribute value by the name
         * @param output value
         * @return attribute value
         */
        template<class T>
        T& ValueAttribute(const std::string& name, T& value) const
        {
            int res = QueryNode()->QueryValueAttribute(name, &value);
            if ( res == TIXML_WRONG_TYPE ) {
                throw DOMException("Wrong attribute type");
            }
            else if ( res == TIXML_NO_ATTRIBUTE ) {
                throw DOMException("Attribute not found");
            }
            return value;
        }

        /**
         * Get the first attribute iterator
         * @return iterator addressing first attribute
         */
        attribute_iterator          FirstAttribute();

        /**
         * Get the first attribute const iterator
         * @return const iterator addressing first attribute
         */
        const_attribute_iterator    FirstAttribute() const;

        /**
         * Get the end attribute iterator
         * @return iterator addressing attribute after last attribute
         */
        attribute_iterator          EndAttribute();

        /**
         * Get the end attribute const iterator
         * @return const iterator addressing attribute after last attribute
         */
        const_attribute_iterator    EndAttribute() const;
    };
}

#endif // ELEMENT_h
