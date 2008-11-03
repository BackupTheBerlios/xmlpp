#include "Element.h"

using namespace xmlpp;

/**
 * Get TiXmlNode
 * @return TiXmlNode it wraps
 */
TiXmlNode* Node::ToTiXmlNode() const
{
    return pNode;
}

/**
 * Value of the xml element
 * @return value of the xml element
 */
std::string Node::Value() const
{
    assert(pNode);
    return pNode->Value();
}

/**
 * Setup new value to the element
 * @param new value
 */
void Node::SetValue(const std::string& newVal)
{
    assert(pNode);
    pNode->SetValue(newVal);
}

/**
 * One step up the DOM
 * @return iterator addresing parent element
 */
const_element_iterator Node::Parent() const
{
    assert(pNode);
	const TiXmlNode* parent = pNode->Parent(); 
	if (parent) {
		return const_element_iterator( const_cast<TiXmlElement*>(parent->ToElement()) );
	}
	return const_element_iterator();
}

/**
 * One step up the DOM
 * @return iterator addresing parent element
 */
element_iterator Node::Parent()
{
    assert(pNode);
	TiXmlNode* parent = pNode->Parent(); 
	if (parent) {
		return element_iterator( parent->ToElement() );
	}
	return element_iterator();
}

/**
 * Move to next sibling
 * @return node addressing next sibling
 */
Node Node::NextSibling() const
{
    assert(pNode);
    return Node( pNode->NextSibling() );
}

/**
 * Move to previous sibling
 * @return node addressing previous sibling
 */
Node Node::PreviousSibling() const
{
    assert(pNode);
    return Node( pNode->PreviousSibling() );
}

/**
 * Clear all the children of this node
 */
void Node::Clear()
{
    assert(pNode);
    pNode->Clear();
}

/**
 * Get iterator to the first child node
 * @return iterator addressing first child node
 */
node_iterator Node::FirstChild()
{
    assert(pNode);
    return node_iterator( pNode->FirstChildElement() );
}

/**
 * Get const iterator to the first child node
 * @return const iterator addressing first child node
 */
const_node_iterator Node::FirstChild() const
{
    assert(pNode);
    return const_node_iterator( pNode->FirstChild() );
}

/**
 * Get iterator to the node after last node
 * @return iterator addressing node after last node
 */
node_iterator Node::EndChild()
{
    return node_iterator(NULL);
}

/**
 * Get const iterator to the node after last node
 * @return const iterator addressing node after last node
 */
const_node_iterator Node::EndChild() const
{
    return const_node_iterator(NULL);
}

/**
 * Get iterator to the first child node
 * @return iterator addressing first child node
 */
element_iterator Node::FirstChildElement(const char* value)
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
const_element_iterator Node::FirstChildElement(const char* value) const
{
    assert(pNode);
    TiXmlElement* pElem = value ? pNode->FirstChildElement(value)
                                : pNode->FirstChildElement();

    return const_element_iterator(pElem);
}

/**
 * Get iterator to the node after last element
 * @return iterator addressing node after last element
 */
element_iterator Node::EndChildElement()
{
    return element_iterator(NULL);
}

/**
 * Get const iterator to the node after last element
 * @return const iterator addressing node after last element
 */
const_element_iterator Node::EndChildElement() const
{
    return const_element_iterator(NULL);
}

/**
 * Compare nodes
 * @value other node
 * @return true if nodes are equal
 */
bool Node::operator == (const Node& rhs) const
{
    return pNode == rhs.pNode;
}
