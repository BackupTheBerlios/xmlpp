#ifndef XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP

#include "../element.h"
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>

namespace xmlpp {

/** class serializes/deserializes object from the stream using operators <<, >> */
template<typename T, typename Y = T>
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

    /** Check for validness */
    bool valid() const { return true; }

private:
    T& item;
};

/** class serializer/deserializes object to the element */
template<typename T, typename Y = T>
class element_serializer;

template<typename T>
class element_serializer<T, T>
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

    /** Check for validness */
    bool valid() const { return true; }

private:
    T& serializer;
};

/** Specialization for serialization/deserialization ptr to object */
template<typename T>
class element_serializer<T*,T*>
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer(T* serializer_) :
        serializer(serializer_)
    {
        assert(serializer);
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        assert(serializer);
        serializer->load(d, e);
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        assert(serializer);
        serializer->save(d, e);
    }

    /** Check for validness */
    bool valid() const { return (serializer != 0); }

private:
    T* serializer;
};

/** Specialization for serialization/deserialization shared_ptr to object */
template<typename T>
class element_serializer< boost::shared_ptr<T>, boost::shared_ptr<T> >
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer(const boost::shared_ptr<T>& serializer_) :
        serializer(serializer_) 
    {
        assert(serializer);
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        assert(serializer);
        serializer->load(d, e);
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        assert(serializer);
        serializer->save(d, e);
    }

    /** Check for validness */
    bool valid() const { return (serializer.get() != 0); }

private:
    boost::shared_ptr<T> serializer;
};

/** Specialization for serialization/deserialization ptr to base object */
template<typename T, typename Y>
class element_serializer<T*,Y*>
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer(T* serializer_)
    {
        assert(serializer_);
        serializer = dynamic_cast<Y*>(serializer_);
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        if (serializer) {
            serializer->load(d, e);
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) {
            serializer->save(d, e);
        }
    }

    /** Check for validness */
    bool valid() const { return (serializer != 0); }

private:
    Y* serializer;
};

/** Specialization for serialization/deserialization shared_ptr to object */
template<typename T, typename Y>
class element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y> >
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer(const boost::shared_ptr<T>& serializer_)
    {
        serializer = boost::shared_dynamic_cast<Y>(serializer_);
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        if (serializer) {
            serializer->load(d, e);
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) {
            serializer->save(d, e);
        }
    }

    /** Check for validness */
    bool valid() const { return (serializer.get() != 0); }

private:
    boost::shared_ptr<Y> serializer;
};

template<typename T>
text_serializer<T,T> as_text(T& item)
{
    return text_serializer<T,T>(item);
}

template<typename T>
element_serializer<T,T> as_element(T& item)
{
    return element_serializer<T,T>(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
