#ifndef XMLPP_DOCUMENT_H
#define XMLPP_DOCUMENT_H

#include <string>
#include <boost/shared_ptr.hpp>
#include "tinyxml.h"
#include "element.h"

namespace xmlpp {

/**
 * Exception occuring then there are problems during io operations,
 * e.g. file not found, or you have insufficent permissions 
 */
class io_error :
    public std::runtime_error
{
public:
    io_error() : 
        std::runtime_error("IO error") {}
    io_error(const io_error& rhs) : 
        std::runtime_error(rhs) {}
    explicit io_error(const std::string& msg) : 
        std::runtime_error(msg) {}
};

/**
 * Base class for parsing xml documents.
 */
class document
{
public:
    document();
    document(const document& rhs);
    explicit document(const std::string& source);
    virtual ~document();

    /** Set document source
     * @param source - string containing xml file
     */
    void set_source(const std::string& source);

    /** Set document source
     * TODO: Split file errors & dom errors
     * @param fileName - name of the xml formatted file
     */
    void set_file_source(const std::string& fileName, TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING);

    /** Does nothing. Overload this function to implement required
     *  actions after the document has been loaded.
     */
    virtual void on_load() {}

    /** Get tiny xml document. Use with care. */
    TiXmlDocument* get_tixml_document() { return tixmlDocument.get(); }

    /** Get iterator to the first child node
     * @return iterator addressing first child node
     */
    element_iterator first_child_element()
    {
        assert(tixmlDocument);
        return element_iterator( tixmlDocument->FirstChildElement() );
    }

    /** Get const iterator to the first child element
     * @return const iterator addressing first child element
     */
    const_element_iterator first_child_element() const
    {
        assert(tixmlDocument);
        return const_element_iterator( tixmlDocument->FirstChildElement() );
    }

    /** Get iterator to the first child element with the specified name
     * @param name - name of the element
     * @return iterator addressing first child element with specified name
     */
    element_iterator first_child_element(const std::string& name);

    /** Get iterator to the first child element with the specified name
     * @param name - name of the element
     * @return iterator addressing first child element with specified name
     */
    const_element_iterator first_child_element(const std::string& name) const;

    /**
     * Get iterator to the node after last element
     * @return iterator addressing node after last element
     */
    element_iterator end_child_element()  { return element_iterator(NULL); }

    /**
     * Get const iterator to the node after last element
     * @return const iterator addressing node after last element
     */
    const_element_iterator end_child_element() const { return const_element_iterator(NULL); }

private:
    boost::shared_ptr<TiXmlDocument> tixmlDocument;
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

#endif // XMLPP_DOCUMENT_H
