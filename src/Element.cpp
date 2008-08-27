#include "Element.h"

using namespace xmlpp;

/**
 * Text of the element( Same as TiXmlElement::GetText() )
 * @return text of the element
 */
std::string Element::Text() const
{
    assert(pNode);
    return QueryNode()->GetText();
}

/**
 * Set text of the element.
 * For example: There is <b></b> tag. After SetText("txt") it became <b>txt</b>.
 * Example2: <b><d/></b> -> <b>txt<d/></b>
 * Example3: <b><d/>txt2</b> -> <b>txt<d/></b>
 * @param text of the element
 */
void Element::SetText(const std::string text)
{
    assert(pNode);

    bool updated = false;
    node_iterator i = FirstChild();
    while( i != EndChild() )
    {
        if ( TiXmlText* textNode = i->ToTiXmlNode()->ToText() )
        {
            if ( !updated )
            {
                textNode->SetValue(text);
                updated = true;
                ++i;
            }
            else
            {
                i = i->NextSibling();
                pNode->RemoveChild(textNode);
            }
        }
        else {
            ++i;
        }
    }
}

/**
 * Check if element has attribute with specified name
 * @param name of the attribute
 * @return true if has
 */
bool Element::HasAttribute(const std::string& name) const
{
    assert(pNode);
    const char* result = QueryNode()->Attribute( name.c_str() );
    return result != NULL;
}

/**
 * Get attribute by the name
 * @return value of the attribute with specified name
 */
std::string Element::Attr(const std::string& name) const
{
    assert(pNode);
    const char* result = QueryNode()->Attribute( name.c_str() );
    if (!result) {
        throw DOMException("Attribute '" + name + "' not found");
    }
    return result;
}

/**
 * Get the first attribute iterator
 * @return iterator addressing first attribute
 */
Element::attribute_iterator Element::FirstAttribute()
{
    assert(pNode);
    return attribute_iterator( QueryNode()->FirstAttribute() );
}

/**
 * Get the first attribute const iterator
 * @return const iterator addressing first attribute
 */
Element::const_attribute_iterator Element::FirstAttribute() const
{
    assert(pNode);
    return const_attribute_iterator( const_cast<TiXmlAttribute*>( QueryNode()->FirstAttribute() ) );
}

/**
 * Get the end attribute iterator
 * @return iterator addressing attribute after last attribute
 */
Element::attribute_iterator Element::EndAttribute()
{
    assert(pNode);
    return attribute_iterator(NULL);
}

/**
 * Get the end attribute const iterator
 * @return const iterator addressing attribute after last attribute
 */
Element::const_attribute_iterator Element::EndAttribute() const
{
    assert(pNode);
    return const_attribute_iterator(NULL);
}

/**
 * Move to next sibling
 * @return element addressing next sibling
 */
Element Element::NextSiblingElement() const
{
    assert(pNode);
    return Element( pNode->NextSiblingElement() );
}
