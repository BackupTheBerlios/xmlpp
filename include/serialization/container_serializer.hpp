#ifndef XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP

#include "generic_serializer.hpp"
#include "element_serializer.hpp"
#include <iterator>
#include <vector>

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

template< typename OutIterator,
          typename Serializer,
          typename ValueType,
          typename Constructor = default_constructor<ValueType> >
class container_loader
{
public:
    typedef element xmlpp_holder_type;

public:
    container_loader( OutIterator outIter_, 
                      Constructor constructor_ = Constructor() ) :
        outIter(outIter_),
        constructor(constructor_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        ValueType value = constructor();
        Serializer(value).load(d, e);
        (*outIter++) = value;
    }

public:
    OutIterator outIter;
    Constructor constructor;
};

template<typename InIterator, 
         typename Serializer>
class container_saver
{
public:
    typedef element xmlpp_holder_type;

public:
    container_saver(InIterator firstIter_, InIterator endIter_) :
        firstIter(firstIter_),
        endIter(endIter_)
    {}

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        xmlpp::element_iterator holder(e);
        for (InIterator iter  = firstIter;
                        iter != endIter;
                        ++iter)
        {
            Serializer serializer(*iter);
            if ( serializer.valid() ) {
                serializer.save(d, *holder++);
            }
        }
    }

public:
    InIterator  firstIter;
    InIterator  endIter;
};

template< typename InIterator, 
          typename Serializer >
struct generic_holder< name_value_pair< container_saver<InIterator, 
                                                        Serializer> >, 
                       xmlpp::element >
{
    typedef name_value_pair< container_saver<InIterator, 
                                             Serializer> > nvp_type;

    element operator () (const nvp_type& nvp, node& parent)
    {
        if ( nvp.serializer.firstIter != nvp.serializer.endIter)
        {
            // remember first child
            element first(nvp.name);
            add_child(parent, first);

            // allocate children for every object in the sequence
            for (InIterator iter  = nvp.serializer.firstIter, ++iter;
                            iter != nvp.serializer.endIter;
                            ++iter)
            {
                element child(nvp.name);
                add_child(parent, child);
            }

            return first;
        }

        return element("null");
    }
};

template<typename InIterator, 
         typename OutIterator, 
         typename ValueType,
         typename Serializer,
         typename Constructor = default_constructor<ValueType> >
class container_serializer :
    public container_saver<InIterator, Serializer>,
    public container_loader<OutIterator, Serializer, ValueType, Constructor>
{
public:
    typedef element xmlpp_holder_type;

public:
    container_serializer( InIterator         firstIter_,
                          InIterator         endIter_,
                          OutIterator        outIter_,
                          Constructor        constructor_ = Constructor() ) :
        container_saver<InIterator, Serializer>(firstIter_, endIter_),
        container_loader<OutIterator, Serializer, ValueType, Constructor>(outIter_, constructor_)
    {}
};

template< typename InIterator, 
          typename OutIterator, 
          typename ValueType,
          typename Serializer,
          typename Constructor >
struct generic_holder< name_value_pair< container_serializer<InIterator, 
                                                             OutIterator,
                                                             ValueType,
                                                             Serializer,
                                                             Constructor> >, 
                       xmlpp::element >
{
    typedef name_value_pair< container_serializer<InIterator, 
                                                  OutIterator,
                                                  ValueType,
                                                  Serializer,
                                                  Constructor> > nvp_type;

    element operator () (const nvp_type& nvp, node& parent)
    {
        if ( nvp.serializer.firstIter != nvp.serializer.endIter)
        {
            // remember first child
            element first(nvp.name);
            add_child(parent, first);

            // allocate children for every object in the sequence
            InIterator iter = nvp.serializer.firstIter;
            for (++iter; iter != nvp.serializer.endIter; ++iter)
            {
                element child(nvp.name);
                add_child(parent, child);
            }

            return first;
        }

        return element("null");
    }
};

// Specialize for saving containers of pointers
template< typename InIterator, 
          typename OutIterator, 
          typename T,
          typename Y,
          typename Constructor >
struct generic_holder< name_value_pair< container_serializer< InIterator, 
                                                              OutIterator,
                                                              T,
                                                              element_serializer<T, Y>,
                                                              Constructor > >, 
                       xmlpp::element >
{
    typedef name_value_pair< container_serializer< InIterator, 
                                                   OutIterator,
                                                   T,
                                                   element_serializer<T, Y>,
                                                   Constructor > > nvp_type;

    element operator () (const nvp_type& nvp, node& parent)
    {
        element first = element("null");
        bool    empty = true;

        // make elements only for valid serializers
        for (InIterator iter  = nvp.serializer.firstIter; 
                        iter != nvp.serializer.endIter; 
                        ++iter)
        {
            if ( element_serializer<T,Y>(*iter).valid() )
            {
                element child(nvp.name);
                add_child(parent, child);

                if (empty) 
                {
                    first = child;
                    empty = false;
                }
            }
        }

        return first;
    }
};

template<typename ValueType, typename OutIterator>
name_value_pair
<
    container_loader< OutIterator,
                      ValueType,
                      element_serializer<ValueType> > 
>
as_element_set(const std::string& elementName, OutIterator out)
{
    typedef container_loader< OutIterator,
                              ValueType,
                              element_serializer<ValueType> > serializer;

    return name_value_pair<serializer>( elementName, serializer(out) );
}

template<typename InIterator>
name_value_pair
<
    container_saver< InIterator,
                     element_serializer<typename InIterator::value_type> > 
>
as_element_set(const std::string& elementName, InIterator begin, InIterator end)
{
    typedef container_saver< InIterator,
                             element_serializer<typename InIterator::value_type> > serializer;

    return name_value_pair<serializer>( elementName, serializer(begin, end) );
}

template<typename ValueType, typename InIterator, typename OutIterator>
name_value_pair
<
    container_serializer< InIterator,
                          OutIterator,
                          ValueType,
                          element_serializer<ValueType> > 
>
as_element_set(const std::string& elementName, InIterator begin, InIterator end, OutIterator out)
{
    typedef container_serializer< InIterator,
                                  OutIterator,
                                  ValueType,
                                  element_serializer<ValueType> > serializer;

    return name_value_pair<serializer>( elementName, serializer(begin, end, out) );
}

template<typename T>
name_value_pair
<
    container_serializer
    < 
        typename std::vector<T>::iterator,
        std::back_insert_iterator< std::vector<T> >,
        T,
        element_serializer<T, T> 
    >
>
as_element_set(const std::string& elementName, std::vector<T>& values)
{
    typedef container_serializer< typename std::vector<T>::iterator,
                                  std::back_insert_iterator< std::vector<T> >,
                                  T,
                                  element_serializer<T, T> > serializer;

    return name_value_pair<serializer>( elementName, serializer( values.begin(), values.end(), std::back_inserter(values) ) );
}

template<typename Y, typename T>
name_value_pair
<
    container_serializer
    < 
        typename std::vector<T*>::iterator,
        std::back_insert_iterator< std::vector<T*> >,
        T*,
        element_serializer<T*, Y*>,
        default_constructor<Y*>
    >
>
as_element_set(const std::string& elementName, std::vector<T*>& values)
{
    typedef container_serializer< typename std::vector<T*>::iterator,
                                  std::back_insert_iterator< std::vector<T*> >,
                                  T*,
                                  element_serializer<T*, Y*>,
                                  default_constructor<Y*> > serializer;

    return name_value_pair<serializer>( elementName, serializer( values.begin(), values.end(), std::back_inserter(values) ) );
}

template<typename Y, typename T>
name_value_pair
<
    container_serializer
    < 
        typename std::vector< boost::shared_ptr<T> >::iterator,
        std::back_insert_iterator< std::vector< boost::shared_ptr<T> > >,
        boost::shared_ptr<T>,
        element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y> >,
        default_constructor< boost::shared_ptr<Y> > 
    >
>
as_element_set(const std::string& elementName, std::vector< boost::shared_ptr<T> >& values)
{
    typedef container_serializer< typename std::vector< boost::shared_ptr<T> >::iterator,
                                  std::back_insert_iterator< std::vector< boost::shared_ptr<T> > >,
                                  boost::shared_ptr<T>,
                                  element_serializer< boost::shared_ptr<T>, boost::shared_ptr<Y> >,
                                  default_constructor< boost::shared_ptr<Y> > > serializer;

    return name_value_pair<serializer>( elementName, serializer( values.begin(), values.end(), std::back_inserter(values) ) );
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
