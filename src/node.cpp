#include "element.h"

namespace xmlpp {

node::node() :
	tixmlNode(0)
{
}

node::node(const node& rhs) : 
    tixmlNode(rhs.tixmlNode) 
{}

node::node(TiXmlNode* _tixmlNode) : 
    tixmlNode(_tixmlNode)
{
}

std::string node::get_value() const
{
    assert(tixmlNode);
    return tixmlNode->Value();
}

void node::set_value(const std::string& newVal)
{
    assert(tixmlNode);
    tixmlNode->SetValue(newVal);
}

const_element_iterator node::get_parent() const
{
    assert(tixmlNode);
	const TiXmlNode* parent = tixmlNode->Parent();
	if (parent) {
		return const_element_iterator( const_cast<TiXmlElement*>( parent->ToElement() ) );
	}
	return const_element_iterator();
}

element_iterator node::get_parent()
{
    assert(tixmlNode);
	TiXmlNode* parent = tixmlNode->Parent();
	if (parent) {
		return element_iterator( parent->ToElement() );
	}
	return element_iterator();
}

node_iterator node::first_child()
{
    assert(tixmlNode);
    return node_iterator( tixmlNode->FirstChildElement() );
}

const_node_iterator node::first_child() const
{
    assert(tixmlNode);
    return const_node_iterator( tixmlNode->FirstChild() );
}

node_iterator node::end_child()
{
    return node_iterator(NULL);
}

const_node_iterator node::end_child() const
{
    return const_node_iterator(NULL);
}

element_iterator node::first_child_element()
{
    assert(tixmlNode);
    TiXmlElement* pElem = tixmlNode->FirstChildElement();
    return element_iterator(pElem);
}

const_element_iterator node::first_child_element() const
{
    assert(tixmlNode);
    TiXmlElement* pElem = tixmlNode->FirstChildElement();
    return const_element_iterator(pElem);
}

element_iterator node::first_child_element(const std::string& value)
{
    assert(tixmlNode);
    TiXmlElement* pElem = tixmlNode->FirstChildElement(value);
    return element_iterator(pElem);
}

const_element_iterator node::first_child_element(const std::string& value) const
{
    assert(tixmlNode);
    TiXmlElement* pElem = tixmlNode->FirstChildElement(value);
    return const_element_iterator(pElem);
}

element_iterator node::end_child_element()
{
    return element_iterator(NULL);
}

const_element_iterator node::end_child_element() const
{
    return const_element_iterator(NULL);
}

bool node::operator == (const node& rhs) const
{
    return tixmlNode == rhs.tixmlNode;
}

////////////////////////////////////////////////////////////////////////

void replace_node( node& what,
                   node& with )
{                         
    assert( what.get_parent() && with.get_tixml_node() );

    TiXmlNode* parentNode = what.get_parent()->get_tixml_node();
    what.set_tixml_node( parentNode->ReplaceChild( what.get_tixml_node(), *with.get_tixml_node() ) );

    if ( !what.get_tixml_node() ) {
        throw dom_error("Error replacing xml node");
    }           
}

void insert_before_node( node& beforeThis,
                         node& insertNode )
{  
    assert( beforeThis.get_parent() && insertNode.get_tixml_node() );

    TiXmlNode* parentNode = beforeThis.get_parent()->get_tixml_node();
    insertNode.set_tixml_node( parentNode->InsertBeforeChild( beforeThis.get_tixml_node(), *insertNode.get_tixml_node() ) );

    if ( !insertNode.get_tixml_node() ) {
        throw dom_error("Error inserting before xml node");
    }
}

void insert_after_node( node& afterThis,
                        node& insertNode )
{
    assert( afterThis.get_parent() && insertNode.get_tixml_node() );

    TiXmlNode* parentNode = afterThis.get_parent()->get_tixml_node();
    afterThis.set_tixml_node( parentNode->InsertAfterChild( afterThis.get_tixml_node(), *insertNode.get_tixml_node() ) );

    if ( !afterThis.get_tixml_node() ) {
        throw dom_error("Error inserting after xml node");
    }
}

void add_child( node& parent,
                node& child )
{        
    assert( parent.get_tixml_node() );

    child.set_tixml_node( parent.get_tixml_node()->LinkEndChild( child.get_tixml_node() ) );

    if ( !child.get_tixml_node() ) {
        throw dom_error("Error adding child node");
    } 
}

void remove_node( node& what )
{
    assert( what.get_parent() );

    bool res = what.get_parent()->get_tixml_node()->RemoveChild( what.get_tixml_node() );

    if (!res) {
        throw dom_error("Error removing xml node");
    }            
}

} // namespace xmlpp