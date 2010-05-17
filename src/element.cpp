#include "element.h"
#include <algorithm>
#include <boost/bind.hpp>

using namespace xmlpp;

element::element() :
    node_impl<TiXmlElement>(0) 
{}

element::element(const element& rhs) :
    node_impl<TiXmlElement>(rhs) 
{}

element::element(TiXmlElement* _tixmlElement) :
    node_impl<TiXmlElement>(_tixmlElement)
{}

element::element(const std::string& value) :
    node_impl<TiXmlElement>( new TiXmlElement(value) )
{}

element::element(const node& elementNode) :
    node_impl<TiXmlElement>( const_cast<node&>(elementNode).get_tixml_node()->ToElement() ) 
{
    if ( !tixmlNode ) {
        throw dom_error("Can't convert node to element");
    }
}

std::string element::get_text() const
{
    assert(tixmlNode);
    const char* text = query_node()->GetText();
    if (text) {
        return std::string(text);
    }

    return "";
}

void element::set_text(const std::string text)
{
    assert(tixmlNode);

    bool updated = false;
    node_iterator i = first_child();
    while( i != end_child() )
    {
        if ( TiXmlText* textNode = i->get_tixml_node()->ToText() )
        {
            if ( !updated )
            {
                textNode->SetValue(text);
                updated = true;
                ++i;
            }
            else
            {
                ++i;
                tixmlNode->RemoveChild(textNode);
            }
        }
        else {
            ++i;
        }
    }

    if ( i == end_child() ) 
    {
        TiXmlText* textNode = new TiXmlText(text);
        tixmlNode->LinkEndChild(textNode);
    }
}

bool element::has_attribute(const std::string& name) const
{
    assert(tixmlNode);
    const char* result = query_node()->Attribute( name.c_str() );
    return result != NULL;
}

void element::set_attribute(const std::string& name, const std::string& text)
{
    assert(tixmlNode);
    query_node()->SetAttribute(name, text);
}

std::string element::get_attribute(const std::string& name) const
{
    assert(tixmlNode);
    const char* result = query_node()->Attribute( name.c_str() );
    if (!result) {
        throw dom_error("Attribute '" + name + "' not found");
    }
    return result;
}

element::attribute_iterator element::first_attribute()
{
    assert(tixmlNode);
    return attribute_iterator( query_node()->FirstAttribute() );
}

element::const_attribute_iterator element::first_attribute() const
{
    assert(tixmlNode);
    return const_attribute_iterator( const_cast<TiXmlAttribute*>( query_node()->FirstAttribute() ) );
}

element::attribute_iterator element::first_attribute(const std::string& name)
{
    return std::find_if( first_attribute(),
                         end_attribute(),
                         boost::bind(&attribute::get_name, _1) == name );
}

element::const_attribute_iterator element::first_attribute(const std::string& name) const
{
    return std::find_if( first_attribute(),
                         end_attribute(),
                         boost::bind(&attribute::get_name, _1) == name );
}

element::attribute_iterator element::end_attribute()
{
    assert(tixmlNode);
    return attribute_iterator(NULL);
}

element::const_attribute_iterator element::end_attribute() const
{
    assert(tixmlNode);
    return const_attribute_iterator(NULL);
}
