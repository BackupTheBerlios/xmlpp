#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/ice.hpp>
#include "Document.h"

namespace xmlpp
{
    /** Error during serialization */
    class ParserError :
        public std::runtime_error
    {
    public:
        ParserError() : std::runtime_error("Error during parsing") {}
        ParserError(const std::string& err) :
            std::runtime_error(err) {}
    };

    /** Interface for serializable objects */
    template<class D>
    class Serializable
    {
    public:
        /** Serialize item to the node */
        virtual void Save(D& document, Node& node) const = 0;

        virtual ~Serializable() {}
    };

    /** Interface for deserializable objects */
    template<class D>
    class Deserializable
    {
    public:
        /** Deserialize item from the node */
        virtual void Load(const D& document, const Node& node) = 0;

        virtual ~Deserializable() {}
    };

    /** class deserializes object from the stream using operator >> */
    template<class T, class D>
    class stream_deserializer
    {
    private:
        T*  item;

    public:
        stream_deserializer(T* _item) :
            item(_item) {}
        stream_deserializer(const stream_deserializer& ss) :
            item(ss.item) {}

        /** serialize item into the stream */
        void operator() (const D& document, const Node& node) const
        {
            std::istringstream sstr( Element(node).Text() );
            sstr >> *item;
        }
    };

    /** class serializes object to the stream using operator << */
    template<class T, class D>
    class stream_serializer
    {
    private:
        T const*    item;

    public:
        stream_serializer(T const* _item) :
            item(_item) {}
        stream_serializer(const stream_serializer& ss) :
            item(ss.item) {}

        /** serialize item into the stream */
        void operator() (D& document, Node& node) const
        {
            std::istringstream sstr;
            sstr << *item;
            Element(node).SetText( sstr.str() );
        }
    };

    /** Create serializer for container serialization
     * @param container to serialize
     * @return serializer for container serialization
     */
    template<class CT, class D>
    boost::function<void (D&, Node&)> container_s(const CT& container)
    {
        return for_each( container.begin(),
                          container.end(),
                          bind(&Serializable<D>::Save, _1, _2, _3) );
    }

    /** Create serializer for container serialization
     * @param container to serialize
     * @param item serializer for items in the container
     * @return serializer for container serialization
     */
    template<class CT, class D, class S>
    boost::function<void (D&, Node&)> container_s( const CT& container,
                                                    const S& s )
    {
        return for_each( container.begin(),
                          container.end(),
                          bind(s, _1, _2, _3) );
    }
    /** Serialize object into the stream
     * @param object to deserialize
     * @param xml document
     * @param xml element containing item
     *
    template<class T, class D>
    void save_to_stream(T* item, const D& document, const Node& node)
    {
        std::istringstream sstr;
        sstr << *item;
        Element(node).SetText( sstr.str() );
    }*/

    /** Save serializable
     * @param serializable object
     * @param xml document
     * @param xml element for item
     */
    template<class D>
    void save(Deserializable<D> const* item, D& document, Node& node)
    {
        item->Save(document, node);
    }

    /** Load object from the stream
     * @param object to deserialize
     * @param xml document
     * @param xml element containing item
     *
    template<class T, class D>
    void load_from_stream(T* item, const D& document, const Node& node)
    {
        std::istringstream sstr( Element(node).Text() );
        sstr >> *item;
    }*/

    /** Load deserializable
     * @param deserializable object
     * @param xml document
     * @param xml element containing item
     */
    template<class D>
    void load(Deserializable<D>* item, const D& document, const Node& node)
    {
        item->Load(document, node);
    }

    /** Get function object for serializing object into the stream */
    template<class D, class T>
    stream_serializer<T,D> stream_s(T const* item)
    {
        return stream_serializer<T,D>(item);
    }

    /** Get function object for deserializing object from the stream */
    template<class D, class T>
    stream_deserializer<T,D> stream_d(T* item)
    {
        return stream_deserializer<T,D>(item);
    }

    /** Generated saver */
    template<class D>
    class GenericSaver
    {
    private:
        typedef boost::function<void (D&, Node&)>   serializer;

        typedef std::pair<std::string, serializer>  serializer_pair;
        typedef std::vector<serializer_pair>        serializer_vector;

        serializer_vector serializers;

    public:
        /** Serializer item to the node */
        void operator() (D& document, Node& node) const
        {
            for(size_t i = 0; i < serializers.size(); ++i)
            {
                Element nameElem(serializers[i].first);
                Element valueElem;

                serializers[i].Save(document, valueElem);

                document.AddChild(nameElem, valueElem);
                document.AddChild(node, nameElem);
            }
        }

        /** Attach serializer
         * @param name of the element to serialize
         * @param serializer to write the element
         */
        void Attach(const std::string& name, const serializer& d)
        {
            serializers.push_back( serializer_pair(name, d) );
        }


        /** Attach item using stream serializer
         * @param name of the element to serialize
         * @param item for serialization
         */
        template<class T>
        void Attach( const std::string& name,
                     T const* item,
                     typename boost::enable_if
                              <
                                  boost::type_traits::ice_not
                                  <
                                      boost::is_convertible< T*, Serializable<D>* >::value
                                  >
                              >::type* dummy = 0 )
        {
            serializers.push_back( serializer_pair( name, stream_serializer<T,D>(item) ) );
        }

        /** Attach serializable item
         * @param name of the element to serialize
         * @param serializable item
         */
        void Attach(const std::string& name, Serializable<D> const* item)
        {
            serializer s = boost::bind(save<D>, item, _1, _2);
            serializers.push_back( serializer_pair(name, s) );
        }
    };

    /** Generated loader */
    template<class D>
    class GenericLoader
    {
    private:
        typedef boost::function<void (const D&, const Node&)> deserializer;

        typedef std::pair<std::string, deserializer>    deserializer_pair;
        typedef std::map<std::string, deserializer>     deserializer_map;

        deserializer_map    deserializers;

    public:
        /** Deserialize item from the node */
        void operator() (const D& document, const Node& node) const
        {
            for( element_iterator i = node.FirstChildElement();
                                  i != node.EndChildElement();
                                  ++i )
            {
                typename deserializer_map::const_iterator j = deserializers.find( i->Value() );
                if ( j == deserializers.end() ) {
                    throw ParserError("Loader for element '" + i->Value() + "' unspecified");
                }

                j->second(document, *i);
            }
        }

        /** Attach deserializer
         * @param name of the element to deserialize
         * @param deserializer to read the element
         */
        void Attach(const std::string& name, const deserializer& d)
        {
            deserializers.insert( deserializer_pair(name, d) );
        }

        /** Attach item using stream deserializer
         * @param name of the element to deserialize
         * @param item for deserialization
         */
        template<class T>
        void Attach( const std::string&     name,
                     T*                     item,
                     typename boost::enable_if
                              <
                                  boost::type_traits::ice_not
                                  <
                                      boost::is_convertible< T*, Deserializable<D>* >::value
                                  >
                              >::type* dummy = 0 )
        {
            deserializers.insert( deserializer_pair(name, stream_deserializer<T,D>(item)) );
        }

        /** Attach deserializable item
         * @param name of the element to deserialize
         * @param deserializable item
         */
        void Attach(const std::string& name, Deserializable<D>* item)
        {
            deserializer d = boost::bind(load<D>, item, _1, _2);
            deserializers.insert( deserializer_pair(name, d) );
        }
    };
}

#endif // SERIALIZATION_HPP
