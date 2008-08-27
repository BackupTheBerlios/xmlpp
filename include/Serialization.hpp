#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "Document.h"

namespace xmlpp
{
    /** Error during serialization */
    class SerializationError :
        public std::runtime_error
    {
    public:
        SerializationError() : std::runtime_error("Error during serialization") {}
        SerializationError(const std::string& err) :
            std::runtime_error(err) {}
    };

    /** Interface for serializable objects */
    template<class D>
    class Serializable
    {
    public:
        /** Serialize item to the node */
        virtual void Serialize(D& document, Node& node) const = 0;

        virtual ~Serializable() {}
    };

    /** Interface for deserializable objects */
    template<class D>
    class Deserializable
    {
    public:
        /** Deserialize item from the node */
        virtual void Deserialize(const D& document, const Node& node) = 0;

        virtual ~Deserializable() {}
    };

    /** Serialize object into the stream
     * @param object to deserialize
     * @param xml document
     * @param xml element containing item
     */
    template<class T, class D>
    void SerializeToStream(T* item, const D& document, const Node& node)
    {
        std::istringstream sstr;
        sstr << *item;
        Element(node).SetText( sstr.str() );
    }

    /** Save serializable
     * @param serializable object
     * @param xml document
     * @param xml element for item
     */
    template<class D>
    void Serialize(Deserializable<D> const* item, D& document, Node& node)
    {
        item->Serialize(document, node);
    }

    /** Load deserializable
     * @param deserializable object
     * @param xml document
     * @param xml element containing item
     */
    template<class D>
    void Deserialize(Deserializable<D>* item, const D& document, const Node& node)
    {
        item->Deserialize(document, node);
    }

    /** Deserialize object from the stream
     * @param object to deserialize
     * @param xml document
     * @param xml element containing item
     */
    template<class T, class D>
    void DeserializeFromStream(T* item, const D& document, const Node& node)
    {
        std::istringstream sstr( Element(node).Text() );
        sstr >> *item;
    }

    /** Get function object for serializing object into the stream */
    template<class D, class T>
    boost::function<void (D&, Node&)> stream_s(T const* item)
    {
        return boost::bind(SerializeToStream<T,D>, item, _1, _2);
    }

    /** Get function object for deserializing object from the stream */
    template<class D, class T>
    boost::function<void (const D&, const Node&)> stream_d(T* item)
    {
        return boost::bind(DeserializeFromStream<T,D>, item, _1, _2);
    }

    /** Generated serializer */
    template<class D>
    class GenericSerializer
    {
    private:
        typedef boost::function<void (D&, Node&)> serializer;

        typedef std::pair<std::string, serializer>  serializer_pair;
        typedef std::vector<serializer_pair>        serializer_vector;

        serializer_vector serializers;

    public:
        /** Deserialize item from the node */
        void operator() (D& document, Node& node) const
        {
            for(size_t i = 0; i < serializers.size(); ++i)
            {
                Element nameElem(serializers[i].first);
                Element valueElem;

                serializers[i].Serialize(document, valueElem);

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
        void AttachStreamS(const std::string& name, T const* item)
        {
            serializers.push_back( serializer_pair( name, stream_s<D,T>(item) ) );
        }

        /** Attach serializable item
         * @param name of the element to serialize
         * @param serializable item
         */
        void Attach(const std::string& name, Serializable<D> const* item)
        {
            serializer d = boost::bind(Serialize<D>, item, _1, _2);
            serializers.push_back( serializer_pair(name, d) );
        }
    };

    /** Generated deserializer */
    template<class D>
    class GenericDeserializer
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
                    throw SerializationError("Serializer for element '" + i->Value() + "' unspecified");
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
        void AttachStreamD(const std::string& name, T* item)
        {
            deserializers.insert( deserializer_pair( name, stream_d<D,T>(item) ) );
        }

        /** Attach deserializable item
         * @param name of the element to deserialize
         * @param deserializable item
         */
        void Attach(const std::string& name, Deserializable<D>* item)
        {
            deserializer d = boost::bind(Deserialize<D>, item, _1, _2);
            deserializers.insert( deserializer_pair(name, d) );
        }
    };
}

#endif // SERIALIZATION_HPP
