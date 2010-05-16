#ifndef XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP

#include "../element.h"
#include <sstream>

namespace xmlpp {

/** class deserializes object from the stream using operator >> */
template<typename T>
class attribute_serializer
{
public:
    typedef attribute xmlpp_holder_type;

public:
    explicit attribute_serializer(T& _item) :
        item(_item) {}

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& /*document*/, const xmlpp_holder_type& a) const
    {
        std::istringstream ss( a.get_value() );
        ss >> item;

        if ( ss.fail() ) {
            throw dom_error("Can't read attribute value");
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(const Document& /*document*/, const xmlpp_holder_type& a) const
    {
        std::ostringstream ss;
        ss << item;

        if ( ss.fail() ) {
            throw dom_error("Can't read attribute value");
        }
        a.set_value( ss.str() );
    }

private:
    T& item;
};

template<typename T>
attribute_serializer<T> as_attr(T& item)
{
    return attribute_serializer<T>(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ATTRIBUTE_SERIALIZER_HPP
