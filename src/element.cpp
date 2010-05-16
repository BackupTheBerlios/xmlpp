#include "element.h"
#include <algorithm>
#include <boost/bind.hpp>

using namespace xmlpp;

std::string element::get_text() const
{
    assert(tixmlNode);
    return query_node()->GetText();
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
