#ifndef XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP

#include "../element.h"
#include "helpers.hpp"
#include <sstream>

namespace xmlpp {

/** Text serialization policy. Reads/Saves object from/to string using std::sringstream. */
template<typename T>
struct text_serialization_policy
{
    typedef element xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp::element& e, T& obj) 
    { 
        std::istringstream ss( e.get_text() );
        ss >> obj;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
    }

    template<typename Document>
    void save(Document& d, xmlpp::element& e, const T& obj) const
    {
        std::ostringstream ss;
        ss << obj;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
        e.set_text( ss.str() );
    }

    bool valid(const T&, s_state) const { return true; }
};

template<typename T>
default_saver
<
    T, 
    text_serialization_policy<T>
> 
to_text(T& item)
{
    typedef default_saver< T, text_serialization_policy<T> > serializer;
    return serializer(item);
}

template<typename T>
default_loader
<
    T, 
    text_serialization_policy<T>
> 
from_text(T& item)
{
    typedef default_loader< T, text_serialization_policy<T> > serializer;
    return serializer(item);
}

template<typename T, typename Constructor>
default_loader
<
    T, 
    text_serialization_policy<T>,
	Constructor
> 
from_text(T& item, Constructor cons)
{
    typedef default_loader<T, text_serialization_policy<T>, Constructor> serializer;
    return serializer(item, cons);
}

template<typename T>
default_serializer
<
    T, 
    text_serialization_policy<T>,
    default_constructor<T>
> 
as_text(T& item)
{
    typedef default_serializer< T, 
                                text_serialization_policy<T>,
                                default_constructor<T> > serializer;

    return serializer(item);
}

template<typename T, typename Constructor>
default_serializer
<
    T, 
    text_serialization_policy<T>,
    Constructor
> 
as_text(T& item, Constructor cons)
{
    typedef default_serializer< T, 
                                text_serialization_policy<T>,
                                Constructor > serializer;

    return serializer(item, cons);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP
