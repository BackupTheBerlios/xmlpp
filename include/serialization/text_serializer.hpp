#ifndef XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP

#include "../element.h"
#include <sstream>

namespace xmlpp {

/** class deserializes object from the stream using operator >> */
template<typename T>
class text_serializer
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit text_serializer(T& _item) :
        item(_item) {}

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& /*document*/, const xmlpp_holder_type& e) const
    {
        std::istringstream ss( e.get_text() );
        ss >> item;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& /*document*/, xmlpp_holder_type& e) const
    {
        std::ostringstream ss;
        ss << item;
        
        if ( ss.fail() ) {
            throw dom_error("Can't read element value.");
        }
        e.set_value( ss.str() );
    }

private:
    T&  item;
};

template<typename T>
text_serializer<T> as_text(T& item)
{
    return text_serializer<T>(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_TEXT_SERIALIZER_HPP
