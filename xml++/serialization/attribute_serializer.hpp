#ifndef XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP

#include "../element.h"
#include "helpers.hpp"
#include <sstream>

namespace xmlpp {
    
/** Attribute serialization policy. Reads/Saves object from/to string using std::sringstream. */
template<typename T>
struct attribute_serialization_policy
{
    typedef attribute xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp::attribute& a, T& obj) 
    { 
        std::istringstream ss( a.get_value() );
        ss >> obj;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
    }

    template<typename Document>
    void save(Document& d, xmlpp::attribute& a, const T& obj) const
    {
        std::ostringstream ss;
        ss << obj;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
        a.set_value( ss.str().c_str() );
    }

    bool valid(const T&, xmlpp::s_state) const { return true; }
};

/** Make attribute loader */
template<typename T>
default_loader
<
    T,
    attribute_serialization_policy<T>,
    default_constructor<T>
> 
from_attribute(T& item)
{
    typedef default_loader< T,
                            attribute_serialization_policy<T>,
                            default_constructor<T> > serializer;

    return serializer(item);
}

/** Make attribute saver */
template<typename T>
default_saver
<
    T,
    attribute_serialization_policy<T>
> 
to_attribute(const T& item)
{
    typedef default_saver< T,
                           attribute_serialization_policy<T> > serializer;

    return serializer(item);
}

/** Make attribute serializer */
template<typename T>
default_serializer
<
    T,
    attribute_serialization_policy<T>,
    default_constructor<T>
> 
as_attribute(T& item)
{
    typedef default_serializer< T,
                                attribute_serialization_policy<T>,
                                default_constructor<T> > serializer;

    return serializer(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP
