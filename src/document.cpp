#include "document.h"
#include <algorithm>

namespace xmlpp {

document::document() :
    node_impl<TiXmlDocument>(new TiXmlDocument)
{
}

document::document(const document& rhs) :
    fileName(rhs.fileName)
{
}

document::document(size_t size, const char* source) :
    node_impl<TiXmlDocument>(new TiXmlDocument)
{
    set_source(size, source);
}

document::~document()
{
    delete tixmlNode;
}

void document::set_source(size_t size, const char* source)
{
    query_node()->Parse(source);
    if ( query_node()->Error() ) {
        throw dom_error( std::string("Parse error: ") + query_node()->ErrorDesc() );
    }
    this->on_load();
}

void document::set_file_source(const std::string& _fileName, TiXmlEncoding encoding)
{
    if ( !query_node()->LoadFile(_fileName, encoding) ) {
        throw file_error( std::string("Loading error: ") + query_node()->ErrorDesc() );
    }
    fileName = _fileName;
    this->on_load();
}

element_iterator document::first_child_element()
{
    TiXmlElement* pElem = query_node()->FirstChildElement();
    return element_iterator(pElem);
}

const_element_iterator document::first_child_element() const
{
    const TiXmlElement* pElem = query_node()->FirstChildElement();
    return const_element_iterator( const_cast<TiXmlElement*>(pElem) );
}

element_iterator document::first_child_element(const char* value)
{
    TiXmlElement* pElem = query_node()->FirstChildElement(value);
    return element_iterator(pElem);
}

const_element_iterator document::first_child_element(const char* value) const
{
    const TiXmlElement* pElem = query_node()->FirstChildElement(value);
    return const_element_iterator( const_cast<TiXmlElement*>(pElem) );
}

} // namespace xmlpp