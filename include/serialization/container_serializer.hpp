#ifndef XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP

#include "generic_serializer.hpp"
#include "element_serializer.hpp"
#include <iterator>

namespace xmlpp {

template<typename OutIterator, 
         typename Serializer,
         typename ValueType,
         typename Holder>
class container_loader
{
public:
    typedef Holder xmlpp_holder_type;

public:
    container_loader(OutIterator outIter_) :
        outIter(outIter_) 
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) const
    {
        ValueType  value;
        Serializer serializer(value);
        serializer.load(d, e);
        (*outIter++) = value;
    }

private:
    OutIterator outIter;
};

template<typename InIterator, 
         typename Serializer,
         typename Holder>
class container_saver
{
public:
    typedef Holder xmlpp_holder_type;

public:
    container_saver(InIterator firstIter_, InIterator endIter_) :
        firstIter(firstIter_),
        endIter(endIter_)
    {}

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        for (InIterator iter  = firstIter;
                        iter != endIter;
                        ++iter)
        {
            Serializer serializer(*iter);
            serializer.save(d, e);
        }
    }

private:
    InIterator firstIter;
    InIterator endIter;
};

template<typename InIterator, 
         typename OutIterator, 
         typename ValueType,
         typename Serializer,
         typename Holder>
class container_serializer :
    public container_saver<InIterator, Serializer, Holder>,
    public container_loader<OutIterator, ValueType, Serializer, Holder>
{
public:
    typedef Holder xmlpp_holder_type;

public:
    container_serializer(InIterator  firstIter_,
                         InIterator  endIter_,
                         OutIterator outIter_) :
        container_saver<InIterator, Serializer>(firstIter_, endIter_),
        container_loader<OutIterator, Serializer>(outIter_)
    {}
};

template<typename ValueType, typename OutIterator>
container_loader<OutIterator,
                 ValueType,
                 element_serializer<ValueType>,
                 element> as_element_set(OutIterator out)
{
    return container_loader<OutIterator,
                            element_serializer<typename OutIterator::value_type>,
                            element>(out);
}

template<typename InIterator>
container_saver<InIterator,
                element_serializer<typename InIterator::value_type>,
                element> as_element_set(InIterator begin, InIterator end)
{
    return container_saver<InIterator,
                           element_serializer<typename InIterator::value_type>,
                           element>(begin, end);
}

template<typename ValueType, typename InIterator, typename OutIterator>
container_serializer<InIterator,
                     OutIterator,
                     ValueType,
                     element_serializer<ValueType>,
                     element> as_element_set(InIterator begin, InIterator end, OutIterator out)
{
    return container_serializer<InIterator,
                                OutIterator,
                                ValueType,
                                element_serializer<ValueType>,
                                element>(begin, end, out);
}

template<typename ValueType, typename VectorValueType>
container_serializer<typename std::vector<VectorValueType>::iterator,
                     std::back_insert_iterator< std::vector<VectorValueType> >,
                     ValueType,
                     element_serializer<ValueType>,
                     element> as_element_set(std::vector<VectorValueType>& values)
{
    return container_serializer< typename std::vector<VectorValueType>::iterator,
                                 std::back_insert_iterator< std::vector<VectorValueType> >,
                                 ValueType,
                                 element_serializer<ValueType>,
                                 element >( values.begin(), values.end(), std::back_inserter(values) );
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
