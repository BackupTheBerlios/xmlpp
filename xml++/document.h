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
class file_error :
    public std::runtime_error
{
public:
    file_error() : 
        std::runtime_error("file error") {}
    file_error(const file_error& rhs) : 
        std::runtime_error(rhs) {}
    explicit file_error(const std::string& msg) : 
        std::runtime_error(msg) {}
};

/**
 * Base class for parsing xml documents.
 */
class document :
    public node_impl<TiXmlDocument>
{
public:
    document();
    document(const document& rhs);
    document(size_t size, const char* source);
    virtual ~document();

    /** Set document source.
	 * @param size - size of the source.
     * @param source - string containing xml file.
     * @throws dom_error
     */
    void set_source(size_t size, const char* source);

    /** Set document source
     * TODO: Split file errors & dom errors
     * @param fileName - name of the xml formatted file
     * @throws dom_error, file_error
     */
    void set_file_source(const std::string& fileName, TiXmlEncoding encoding = TIXML_DEFAULT_ENCODING);

    /** Dump document to file. Also you can use operator <<. */
    void print_file(const std::string& fileName) const;

	/** Print readable xml file to the stream. */
	void print_file(std::ostream& os) const;

    /** Does nothing. Overload this function to implement required
     *  actions after the document has been loaded.
     */
    virtual void on_load() {}

    /** Get tiny xml document. Use with care. */
    TiXmlDocument* get_tixml_document() { return query_node(); }

    /** Get tiny xml document. Use with care. */
    const TiXmlDocument* get_tixml_document() const { return query_node(); }

	/** Add child element to the document. Be carefull - well format xml documents must have single root child */
    void add_child(node& n);

    /** Get iterator to the first child node
     * @return iterator addressing first child node
     */
    element_iterator first_child_element();

    /** Get const iterator to the first child element
     * @return const iterator addressing first child element
     */
    const_element_iterator first_child_element() const;

    /** Get iterator to the first child element with the specified name
     * @param name - name of the element
     * @return iterator addressing first child element with specified name
     */
    element_iterator first_child_element(const char* name);

    /** Get iterator to the first child element with the specified name
     * @param name - name of the element
     * @return iterator addressing first child element with specified name
     */
    const_element_iterator first_child_element(const char* name) const;

    /**
     * Get iterator to the node after last element
     * @return iterator addressing node after last element
     */
    element_iterator end_child_element() { return element_iterator(NULL); }

    /**
     * Get const iterator to the node after last element
     * @return const iterator addressing node after last element
     */
    const_element_iterator end_child_element() const { return const_element_iterator(NULL); }

    /** Get document file name if it has been loaded from the file. Otherwise return emptry string */
    const std::string& get_file_name() const { return fileName; }

private:
    std::string fileName;
};

} // namespace xmlpp

#endif // XMLPP_DOCUMENT_H
