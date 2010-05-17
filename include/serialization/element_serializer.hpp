#ifndef XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP

#include "../element.h"
#include <sstream>

namespace xmlpp {

/** class serializes/deserializes object from the stream using operators <<, >> */
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
    void load(const Document& /*document*/, const xmlpp_holder_type& e)
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
        e.set_text( ss.str() );
    }

private:
    T& item;
};

/** class serializer/deserializes object to the element */
template<typename T>
class element_serializer
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer(T& serializer_) :
        serializer(serializer_) {}

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        serializer.load(d, e);
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        serializer.save(d, e);
    }

private:
    T& serializer;
};

template<typename T>
text_serializer<T> as_text(T& item)
{
    return text_serializer<T>(item);
}

template<typename T>
element_serializer<T> as_element(T& item)
{
    return element_serializer<T>(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
