#include "Document.h"
#include <algorithm>

using namespace std;
using namespace xmlpp;

Document::Document()
    : NodeImpl<TiXmlDocument>(NULL)
{
}

Document::Document(const Document& rhs) :
    NodeImpl<TiXmlDocument>(rhs)
{
}

Document::Document(const std::string& source)
{
    SetSource(source);
}

Document::~Document()
{
    delete pNode;
}

/**
 * Set document source
 * @param string containing xml file
 */
void Document::SetSource(const std::string& source)
{
    pNode = new TiXmlDocument();
    QueryNode()->Parse( source.c_str() );
}

/**
 * Set document source
 * @param string containing xml filename
 */
void Document::SetFileSource(const std::string& source, TiXmlEncoding encoding)
{
    pNode = new TiXmlDocument();
    QueryNode()->LoadFile(source, encoding);
}

/**
 * Get xml document.
 * @return xml document.
 */
TiXmlDocument const& Document::TiDocument() const
{
    return *QueryNode();
}

/**
 * Replace specified node with new node.
 * @param node for replacement
 * @param new node
 */
Node Document::ReplaceNode( Node&   what,
                            Node&   with )
{
    what.pNode = what.Parent()->pNode->ReplaceChild(what.pNode, *with.pNode);

    if (!what.pNode) {
        throw DOMException("Error replacing xml node");
    }

    return what;
}

/**
 * Insert node before specified node.
 * @param before this node will be inserted node
 * @param node to insert
 * @return replaced node
 */
Node Document::InsertNodeBefore( Node&  beforeThis,
                                 Node&  node )
{
    node.pNode = beforeThis.Parent()->pNode->InsertBeforeChild(beforeThis.pNode, *node.pNode);

    if (!node.pNode) {
        throw DOMException("Error inserting before xml node");
    }

    return node;
}

/**
 * Insert node after specified node.
 * @param after this node will be inserted node
 * @param node to insert
 * @return inserted node
 */
Node Document::InsertNodeAfter( Node&   afterThis,
                                Node&   node )
{
    node.pNode = afterThis.Parent()->pNode->InsertAfterChild(afterThis.pNode, *node.pNode);

    if (!node.pNode ) {
        throw DOMException("Error inserting after xml node");
    }

    return node;
}

/**
 * Make node child of another node
 * @param parent node
 * @return child node
 */
Node Document::AddChild( Node&    parent,
                         Node&    child )
{
    assert(parent.pNode);
    child.pNode = parent.pNode->LinkEndChild(child.pNode);

    if (!child.pNode ) {
        throw DOMException("Error adding child node");
    }

    return child;
}

/**
 * Get iterator to the first child node
 * @return iterator addressing first child node
 */
element_iterator Document::FirstChildElement(const char* value)
{
    assert(pNode);
    TiXmlElement* pElem = value ? pNode->FirstChildElement(value)
                                : pNode->FirstChildElement();

    return element_iterator(pElem);
}

/**
 * Get const iterator to the first child element
 * @return const iterator addressing first child element
 */
const_element_iterator Document::FirstChildElement(const char* value) const
{
    assert(pNode);
    TiXmlElement* pElem = value ? pNode->FirstChildElement(value)
                                : pNode->FirstChildElement();

    return const_element_iterator(pElem);
}

/**
 * Remove specified node.
 * @param iterator addressing node for replacement
 * @param new node
 * @return inserted node
 */
void Document::RemoveNode( Node& what )
{
    bool res = what.Parent()->pNode->RemoveChild(what.pNode);

    if (!res) {
        throw DOMException("Error removing xml node");
    }
}

/*
DeviceEffect Graphics::LoadDeviceEffect( const Document& doc,
                                         const ticpp::Element* xmlElem )
{

}

LodEffect Graphics::LoadLodEffect( const Document& doc,
                                   const ticpp::Element* xmlElem )
{

}
*/

