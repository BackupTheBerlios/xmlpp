#ifndef XMLPP_SERIALIZATION_HPP
#define XMLPP_SERIALIZATION_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "document.h"

namespace xmlpp  {

/** Error during serialization */
class parser_error :
    public std::runtime_error
{
public:
    parser_error() : 
        std::runtime_error("Error during parsing") {}
    parser_error(const std::string& err) :
        std::runtime_error(err) {}
};

/** class deserializes object from the stream using operator >> */
template<typename T, typename D>
class stream_deserializer
{
private:
    T*  item;

public:
    stream_deserializer(const stream_deserializer& ss) :
        item(ss.item) {}
    explicit stream_deserializer(T* _item) :
        item(_item) {}

    /** serialize item into the stream */
    void operator() (const D& document, const node& n) const
    {
        std::istringstream sstr( element(n).get_text() );
        sstr >> *item;
    }
};

/** class serializes object to the stream using operator << */
template<typename T, typename D>
class stream_serializer
{
private:
    T const*    item;

public:
    stream_serializer(const stream_serializer& ss) :
        item(ss.item) {}
    explicit stream_serializer(T const* _item) :
        item(_item) {}

    /** serialize item into the stream */
    void operator() (D& document, node& n) const
    {
        std::istringstream sstr;
        sstr << *item;
        element(n).SetText( sstr.str() );
    }
};

/** Create serializer for container serialization
 * @param container - container to serialize
 * @return serializer for container serialization
 */
template<typename CT, typename D>
boost::function<void (D&, node&)> container_s(const CT& container)
{
    return for_each( container.begin(),
                     container.end(),
                     bind(&Serializable<D>::Save, _1, _2, _3) );
}

/** Create serializer for container serialization
 * @param container - container to serialize
 * @param s - item serializer for items in the container
 * @return serializer for container serialization
 */
template<typename CT, typename D, typename S>
boost::function<void (D&, node&)> container_s( const CT& container,
                                               const S& s )
{
    return for_each( container.begin(),
                     container.end(),
                     bind(s, _1, _2, _3) );
}

/** Get function object for serializing object into the stream */
template<typename D, typename T>
stream_serializer<T,D> stream_s(T const* item)
{
    return stream_serializer<T,D>(item);
}

/** Get function object for deserializing object from the stream */
template<typename D, typename T>
stream_deserializer<T,D> stream_d(T* item)
{
    return stream_deserializer<T,D>(item);
}

/** Generated saver */
template<typename D>
class generic_saver
{
private:
    typedef boost::function<void (D&, node&)>   serializer;

    typedef std::pair<std::string, serializer>  serializer_pair;
    typedef std::vector<serializer_pair>        serializer_vector;

    serializer_vector serializers;

public:
    /** Serializer item to the node */
    void operator() (D& document, node& n) const
    {
        for(size_t i = 0; i < serializers.size(); ++i)
        {
            Element nameElem(serializers[i].first);
            Element valueElem;

            serializers[i].Save(document, valueElem);

            document.AddChild(nameElem, valueElem);
            document.AddChild(n, nameElem);
        }
    }

    /** Attach serializer
     * @param name - name of the element to serialize
     * @param s - serializer to write the element
     */
    void attach(const std::string& name, const serializer& s)
    {
        serializers.push_back( serializer_pair(name, s) );
    }

    /** Attach item using stream serializer. 
     * TODO: Write concept check
     * @param name of the element to serialize
     * @param item for serialization
     */
    template<typename T>
    void attach( const std::string& name,
                 const T* item )
    {
        serializers.insert( serializer_pair( name, stream_serializer<T,D>(item) ) );
    }
};

/** Generated loader */
template<typename D>
class generic_loader
{
private:
    typedef boost::function<void (const D&, const node&)> deserializer;

    typedef std::pair<std::string, deserializer>    deserializer_pair;
    typedef std::map<std::string, deserializer>     deserializer_map;

    deserializer_map    deserializers;

public:
    /** Deserialize item from the node */
    void operator() (const D& document, const node& n) const
    {
        for( element_iterator i = n.first_child_element();
                              i != n.end_child_element();
                              ++i )
        {
            typename deserializer_map::const_iterator j = deserializers.find( i->get_value() );
            if ( j != deserializers.end() ) {
				j->second(document, *i);                   
				//throw ParserError("Loader for element '" + i->Value() + "' unspecified");
            }
        }
    }

    /** Attach deserializer
     * @param name - name of the element to deserialize
     * @param d - deserializer to read the element
     */
    void attach(const std::string& name, const deserializer& d)
    {
        deserializers.insert( deserializer_pair(name, d) );
    }

    /** Attach item using stream deserializer.
     * TODO: Write concept check
     * @param name of the element to deserialize
     * @param item for deserialization
     */
    template<typename T>
    void attach( const std::string& name,
                 T* item )
    {
        deserializers.insert( deserializer_pair( name, stream_deserializer<T,D>(item) ) );
    }
};

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_HPP
