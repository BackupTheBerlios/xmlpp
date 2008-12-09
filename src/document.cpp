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

/*
DeviceEffect Graphics::LoadDeviceEffect( const document& doc,
                                         const ticpp::Element* xmlElem )
{

}

LodEffect Graphics::LoadLodEffect( const document& doc,
                                   const ticpp::Element* xmlElem )
{

}
*/

////////////////////////////////////////////////////////////////////////

void xmlpp::replace_node( node& what,
                          node& with )
{                         /*
    what.pNode = what.Parent()->pNode->ReplaceChild(what.pNode, *with.pNode);

    if (!what.pNode) {
        throw dom_error("Error replacing xml node");
    }

    return what;            */
}

void xmlpp::insert_before_node( node& beforeThis,
                                node& insertNode )
{                     /*
    node.pNode = beforeThis.Parent()->pNode->InsertBeforeChild(beforeThis.pNode, *node.pNode);

    if (!node.tixmlNode) {
        throw dom_error("Error inserting before xml node");
    }

    return node;        */
}

/**
 * Insert node after specified node.
 * @param after this node will be inserted node
 * @param node to insert
 * @return inserted node
 */
void xmlpp::insert_after_node( node& afterThis,
                               node& insertNode )
{
    /*
    node.pNode = afterThis.Parent()->pNode->InsertAfterChild(afterThis.pNode, *node.pNode);

    if (!node.pNode ) {
        throw DOMException("Error inserting after xml node");
    }

    return node;*/
}

void xmlpp::add_child( node& parent,
                       node& child )
{                 /*
    assert(parent.pNode);
    child.pNode = parent.pNode->LinkEndChild(child.pNode);

    if (!child.pNode ) {
        throw DOMException("Error adding child node");
    }

    return child;   */
}

void xmlpp::remove_node( node& what )
{              /*
    bool res = what.Parent()->pNode->RemoveChild(what.pNode);

    if (!res) {
        throw DOMException("Error removing xml node");
    }            */
}
