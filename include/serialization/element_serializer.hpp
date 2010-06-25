#ifndef XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP

#include "../element.h"
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>

namespace xmlpp {

template<typename T>
struct default_constructor
{
    T operator () (void) const { return T(); }
};

template<typename T>
struct default_constructor<T*>
{
    T* operator () (void) const { return new T(); }
};

template<typename T>
struct default_constructor< boost::shared_ptr<T> >
{
    boost::shared_ptr<T> operator () (void) const { return boost::shared_ptr<T>(new T); }
};

template<typename T>
struct default_constructor< boost::intrusive_ptr<T> >
{
    boost::intrusive_ptr<T> operator () (void) const { return boost::intrusive_ptr<T>(new T); }
};

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
template< typename T, typename Y = T, typename Constructor = default_constructor<Y> >
class element_serializer;

template<typename T, typename Constructor>
class element_serializer<T, T, Constructor>
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
template<typename T, typename Constructor>
class element_serializer<T*,T*,Constructor>
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer( T*& serializer_, Constructor constructor_ = Constructor() ) :
        serializer(serializer_),
        constructor(constructor_)
    {
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        if (!serializer) 
        {
            serializer = constructor();
            assert(serializer);
        }
        serializer->load(d, e);
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) {
            serializer->save(d, e);
        }
    }

    bool valid() const { return (serializer != 0); }

private:
    T*&         serializer;
    Constructor constructor;
};

/** Specialization for serialization/deserialization shared_ptr to object */
template<typename T, typename Constructor>
class element_serializer< boost::shared_ptr<T>, 
                          boost::shared_ptr<T>, 
                          Constructor >
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer( boost::shared_ptr<T>& serializer_, Constructor constructor_ = Constructor() ) :
        serializer(serializer_),
        constructor(constructor_)
    {
    }

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        if (!serializer) 
        {
            serializer = constructor();
            assert(serializer);
        }
        serializer->load(d, e);
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) {
            serializer->save(d, e);
        }
    }

    bool valid() const { return (serializer.get() != 0); }

private:
    boost::shared_ptr<T>& serializer;
    Constructor           constructor;
};

/** Specialization for serialization/deserialization ptr to base object */
template<typename T, typename Y, typename Constructor>
class element_serializer<T*,Y*,Constructor>
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer( T*& serializer_, Constructor constructor_ = Constructor() ) :
        serializer(serializer_),
        constructor(constructor_)
    {}

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        Y* actual = dynamic_cast<Y*>(serializer);
        if (actual) {
            actual->load(d, e);
        }
        else 
        {
            actual = constructor();
            assert(actual);
            actual->load(d, e);
            serializer = actual;
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) 
        {
            assert( dynamic_cast<Y*>(serializer) );
            static_cast<Y*>(serializer)->save(d, e);
        }
    }

    bool valid() const { return (serializer != 0); }

private:
    T*&         serializer;
    Constructor constructor;
};

/** Specialization for serialization/deserialization shared_ptr to object */
template<typename T, typename Y, typename Constructor>
class element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y>, Constructor >
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit element_serializer( boost::shared_ptr<T>& serializer_, Constructor constructor_ = Constructor() ) :
        serializer(serializer_),
        constructor(constructor_)
    {}

    /** deserialize item from the stream */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        boost::shared_ptr<Y> actual = boost::shared_dynamic_cast<Y>(serializer);
        if (actual) {
            actual->load(d, e);
        }
        else 
        {
            actual = constructor();
            assert(actual);
            actual->load(d, e);
            serializer = actual;
        }
    }

    /** serialize item into the stream */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        if (serializer) 
        {
            assert( boost::shared_dynamic_cast<Y>(serializer) );
            boost::shared_static_cast<Y>(serializer)->save(d, e);
        }
    }

    /** Check for validness */
    bool valid() const { return (serializer.get() != 0); }

private:
    boost::shared_ptr<T>&   serializer;
    Constructor             constructor;
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

template<typename Y, typename T>
element_serializer<T*,Y*> as_element_ptr(T*& item)
{
    return element_serializer<T*,Y*>(item);
}

template<typename Y, typename T>
element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y> > as_element_ptr(boost::shared_ptr<T>& item)
{
    return element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y> >(item);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
