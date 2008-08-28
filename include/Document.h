#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <stdexcept>
#include "Element.h"

namespace xmlpp
{
    /**
     * Base class for parsing xml documents.
     */
    class Document :
        public NodeImpl<TiXmlDocument>
    {
    private:
        std::string documentName;

    public:
        Document();
        Document(const Document& rhs);
        explicit Document(const std::string& source);
        ~Document();

        /**
         * Set document source
         * @param string containing xml file
         */
        void SetSource(const std::string& source);

        /**
         * Set document source
         * @param string containing xml filename
         */
        void SetFileSource(const std::string& source, TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING);

        /**
         * Get xml document.
         * @return xml document.
         */
        TiXmlDocument const& TiDocument() const;

        /**
         * Get root xml element containing item.
         * @return xml element containing item.
         */
        //virtual Element RootElement() const = 0;

        /**
         * Replace specified node with new node.
         * @param node for replacement
         * @param new node
         * @return replaced node
         */
        Node ReplaceNode( Node&  what,
                          Node&  with );

        /**
         * Insert node before specified node.
         * @param before this node will be inserted node
         * @param node to insert
         * @return inserted node
         */
        Node InsertNodeBefore( Node&   beforeThis,
                               Node&   node );

        /**
         * Insert node after specified node.
         * @param after this node will be inserted node
         * @param node to insert
         * @return inserted node
         */
        Node InsertNodeAfter( Node&    afterThis,
                              Node&    node );

        /**
         * Make node child of another node
         * @param parent node
         * @return child node
         */
        Node AddChild( Node&    parent,
                       Node&    child );

        /**
         * Remove specified node.
         * @param node to remove
         */
        void RemoveNode( Node& what );

        /**
         * Get iterator to the first child node
         * @return iterator addressing first child node
         */
        element_iterator FirstChildElement()
        {
            assert(pNode);
            return element_iterator( pNode->FirstChildElement() );
        }

        /**
         * Get const iterator to the first child element
         * @return const iterator addressing first child element
         */
        const_element_iterator FirstChildElement() const
        {
            assert(pNode);
            return const_element_iterator( pNode->FirstChildElement() );
        }

        /**
         * Get iterator to the first child node
         * @return iterator addressing first child node
         */
        element_iterator FirstChildElement(const char* value = NULL);

        /**
         * Get const iterator to the first child element
         * @return const iterator addressing first child element
         */
        const_element_iterator FirstChildElement(const char* value = NULL) const;

        /**
         * Get iterator to the node after last element
         * @return iterator addressing node after last element
         */
        element_iterator EndChildElement()  { return element_iterator(NULL); }

        /**
         * Get const iterator to the node after last element
         * @return const iterator addressing node after last element
         */
        const_element_iterator EndChildElement() const { return const_element_iterator(NULL); }
    };
}

#endif // DOCUMENT_H
