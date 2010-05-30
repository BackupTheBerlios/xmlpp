#include "document.h"
#include <algorithm>

using namespace std;
using namespace xmlpp;

document::document()  :
    node_impl<TiXmlDocument>(new TiXmlDocument)
{
}

document::document(const document& rhs) :
    fileName(rhs.fileName)
{
}

document::~document()
{
    delete tixmlNode;
}

void document::set_source(const std::string& source)
{
    if ( !query_node()->Parse( source.c_str() ) ) {
        throw dom_error( string("Parse error: ") + query_node()->ErrorDesc() );
    }
    this->on_load();
}

void document::set_file_source(const std::string& _fileName, TiXmlEncoding encoding)
{
    if ( !query_node()->LoadFile(_fileName, encoding) ) {
        throw io_error( string("Loading error: ") + query_node()->ErrorDesc() );
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

element_iterator document::first_child_element(const std::string& value)
{
    TiXmlElement* pElem = query_node()->FirstChildElement(value);
    return element_iterator(pElem);
}

const_element_iterator document::first_child_element(const std::string& value) const
{
    const TiXmlElement* pElem = query_node()->FirstChildElement(value);
    return const_element_iterator( const_cast<TiXmlElement*>(pElem) );
}