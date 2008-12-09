#include "Element.h"

using namespace xmlpp;

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
		return const_element_iterator( const_cast<TiXmlElement*>( tixmlNode->ToElement() ) );
	}
	return const_element_iterator();
}

element_iterator node::get_parent()
{
    assert(tixmlNode);
	TiXmlNode* parent = tixmlNode->Parent(); 
	if (parent) {
		return element_iterator( tixmlNode->ToElement() );
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
