#include "document.h"
#include <algorithm>

using namespace std;
using namespace xmlpp;

document::document() 
{
}

document::document(const document& rhs) :
    tixmlDocument(rhs.tixmlDocument)
{
}

document::document(const std::string& source)
{
    set_source(source);
}

document::~document()
{
}

void document::set_source(const std::string& source)
{
    tixmlDocument.reset( new TiXmlDocument() );
    if ( !tixmlDocument->Parse( source.c_str() ) ) {
        throw dom_error( string("Parse error: ") + tixmlDocument->ErrorDesc() );
    }
    this->on_load();
}

void document::set_file_source(const std::string& fileName, TiXmlEncoding encoding)
{
    tixmlDocument.reset( new TiXmlDocument() );
    if ( !tixmlDocument->LoadFile(fileName, encoding) ) 
    {
        throw io_error( string("Loading error: ") + tixmlDocument->ErrorDesc() );
    }
    this->on_load();
}

element_iterator document::first_child_element(const std::string& value)
{
    assert(tixmlDocument);
    TiXmlElement* pElem = tixmlDocument->FirstChildElement(value);
    return element_iterator(pElem);
}

const_element_iterator document::first_child_element(const std::string& value) const
{
    assert(tixmlDocument);
    TiXmlElement* pElem = tixmlDocument->FirstChildElement(value);
    return const_element_iterator(pElem);
}

////////////////////////////////////////////////////////////////////////

void xmlpp::replace_node( node& what,
                          node& with )
{                         
    assert( what.get_parent().exist() && with.get_tixml_node() );

    TiXmlNode* parentNode = what.get_parent()->get_tixml_node();
    what.set_tixml_node( parentNode->ReplaceChild( what.get_tixml_node(), *with.get_tixml_node() ) );

    if ( !what.get_tixml_node() ) {
        throw dom_error("Error replacing xml node");
    }           
}

void xmlpp::insert_before_node( node& beforeThis,
                                node& insertNode )
{  
    assert( beforeThis.get_parent().exist() && insertNode.get_tixml_node() );

    TiXmlNode* parentNode = beforeThis.get_parent()->get_tixml_node();
    insertNode.set_tixml_node( parentNode->InsertBeforeChild( beforeThis.get_tixml_node(), *insertNode.get_tixml_node() ) );

    if ( !insertNode.get_tixml_node() ) {
        throw dom_error("Error inserting before xml node");
    }
}

void xmlpp::insert_after_node( node& afterThis,
                               node& insertNode )
{
    assert( afterThis.get_parent().exist() && insertNode.get_tixml_node() );

    TiXmlNode* parentNode = afterThis.get_parent()->get_tixml_node();
    afterThis.set_tixml_node( parentNode->InsertAfterChild( afterThis.get_tixml_node(), *insertNode.get_tixml_node() ) );

    if ( !afterThis.get_tixml_node() ) {
        throw dom_error("Error inserting after xml node");
    }
}

void xmlpp::add_child( node& parent,
                       node& child )
{        
    assert( parent.get_tixml_node() );

    child.set_tixml_node( parent.get_tixml_node()->LinkEndChild( child.get_tixml_node() ) );

    if ( !child.get_tixml_node() ) {
        throw dom_error("Error adding child node");
    } 
}

void xmlpp::remove_node( node& what )
{
    assert( what.get_parent().exist() );

    bool res = what.get_parent()->get_tixml_node()->RemoveChild( what.get_tixml_node() );

    if (!res) {
        throw dom_error("Error removing xml node");
    }            
}
