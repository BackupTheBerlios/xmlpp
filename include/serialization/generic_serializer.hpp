#ifndef XMLPP_SERIALIZATION_GENERIC_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_GENERIC_SERIALIZER_HPP
 
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <map>
#include <stdexcept>
#include "name_value_pair.hpp"

namespace xmlpp  {

/** Serialization state*/
enum s_state
{
    LOAD,
    SAVE
};

/** Make save/load functions which call this->serialize(document, node, state) */
#define XMLPP_SERIALIZATION_MERGE_MEMBER(Type, Document) \
    void save(Document& d, xmlpp_holder_type& n) const { const_cast<Type*>(this)->serialize(d, n, xmlpp::SAVE); }\
    void load(const Document& d, const xmlpp_holder_type& n) { this->serialize(const_cast<Document&>(d), const_cast<xmlpp_holder_type&>(n), xmlpp::LOAD); }

/** Make serialize function which calls this->load/this->save, dependent on serialization state */
#define XMLPP_SERIALIZATION_SPLIT_MEMBER(Document) \
    void serialize(Document& d, xmlpp_holder_type& n, xmlpp::s_state state)\
    {\
        if (state == xmlpp::LOAD) load(d, n);\
        else save(d, n);\
    }

/** Declare serialize(document, element, state) function and make save/load functions which calls this->serialize */
#define XMLPP_ELEMENT_SERIALIZATION(Type, Document) \
    void serialize(Document& d, xmlpp::element& e, xmlpp::s_state state);\
    void save(Document& d, xmlpp::element& n) const { const_cast<Type*>(this)->serialize(d, n, xmlpp::SAVE); }\
    void load(const Document& d, const xmlpp::element& n) { this->serialize(const_cast<Document&>(d), const_cast<xmlpp::element&>(n), xmlpp::LOAD); }

/** Constructor of the holder for the serialization, e.g. element or attribute. 
 * @tparam Serializer - type of the serialization for use with holder.
 * @tparam Holder - type of the holder(element, attribute, ...) 
 */
template<typename Serializer, typename Holder>
struct generic_holder;

template<typename Serializer>
struct generic_holder< name_value_pair<Serializer>, xmlpp::attribute >
{
    attribute operator () (const name_value_pair<Serializer>& nvp, node& parent)
    {
        element elem(parent);
        elem.set_attribute(nvp.name, "");
        return *elem.first_attribute(nvp.name);
    }
};

template<typename Serializer>
struct generic_holder< name_value_pair<Serializer>, xmlpp::element >
{
    element operator () (const name_value_pair<Serializer>& nvp, node& parent)
    {
        element child(nvp.name);
        add_child(parent, child);
        return child;
    }
};

/** Generated loader & saver. Main class for constructing parsers of the xml chunks.
 * @tparam Document - type of the document for serialization, generally user specified or xmlpp::document.
 */
template<typename Document>
class generic_serializer
{
public:
    typedef element xmlpp_holder_type;

private:
    template<typename Holder>
    class serializer
    {
    public:
        virtual void save(Document&, xmlpp::node&) const = 0;
        virtual void load(const Document&, const Holder&) = 0;

        virtual ~serializer() {}
    };

    template<typename Holder, typename Serializer>
    class serializer_wrapper :
        public serializer<Holder>
    {
    public:
        serializer_wrapper(const Serializer& serializer_) :
            serializer(serializer_)
        {}

        void save(Document& d, xmlpp::node& parent) const       
        {
            generic_holder<Serializer, Holder> holder;
            Holder h = holder(serializer, parent);
            serializer.save(d, h);
        }

        void load(const Document& d, const Holder& n) { serializer.load(d, n); }

    public:
        Serializer serializer;
    };

private:
    typedef boost::shared_ptr< serializer<element> >            element_serializer_ptr;
    typedef std::map<std::string, element_serializer_ptr>       element_serializer_map;
    typedef typename element_serializer_map::iterator           element_serializer_iterator;
    typedef typename element_serializer_map::const_iterator     element_serializer_const_iterator;

    typedef boost::shared_ptr< serializer<attribute> >          attribute_serializer_ptr;
    typedef std::map<std::string, attribute_serializer_ptr>     attribute_serializer_map;
    typedef typename attribute_serializer_map::iterator         attribute_serializer_iterator;
    typedef typename attribute_serializer_map::const_iterator   attribute_serializer_const_iterator;

public:
    /** Save to the document, ignore attributes */
    void save(Document& d) const   
    {
        // save elements
        for( element_serializer_const_iterator i  = elementSerializers.begin();
                                               i != elementSerializers.end();
                                               ++i )
        {
            i->second->save(d, d);
        }
    }

    /** Save to the element. */
    void save(Document& d, xmlpp_holder_type& e) const   
    {
        // save attributes
        for( attribute_serializer_const_iterator i  = attributeSerializers.begin();
                                                 i != attributeSerializers.end();
                                                 ++i )
        {
            i->second->save(d, e);
        }

        // save elements
        for( element_serializer_const_iterator i  = elementSerializers.begin();
                                               i != elementSerializers.end();
                                               ++i )
        {
            i->second->save(d, e);
        }
    }

    /** Load from the document, ignore attributes */
    void load(const Document& d)   
    {
        // read elements
        for( const_element_iterator i  = d.first_child_element();
                                    i != d.end_child_element();
                                    ++i )
        {
            element_serializer_iterator j = elementSerializers.find( i->get_value() );
            if ( j != elementSerializers.end() ) {
				j->second->load(d, *i);
            }
        }
    }

    /** Load from the element */
    void load(const Document& d, const xmlpp_holder_type& e) 
    {
        // read attributes
        for( element::const_attribute_iterator i  = e.first_attribute();
                                               i != e.end_attribute();
                                               ++i )
        {
            attribute_serializer_iterator j = attributeSerializers.find( i->get_name() );
            if ( j != attributeSerializers.end() ) {
				j->second->load(d, *i);
            }
        }

        // read elements
        for( const_element_iterator i  = e.first_child_element();
                                    i != e.end_child_element();
                                    ++i )
        {
            element_serializer_iterator j = elementSerializers.find( i->get_value() );
            if ( j != elementSerializers.end() ) {
				j->second->load(d, *i);
            }
        }
    }

    XMLPP_SERIALIZATION_SPLIT_MEMBER(Document)

#define HOLDER_IS_CONVERTIBLE(To) typename boost::enable_if< boost::is_convertible<typename S::xmlpp_holder_type*, To*> >::type* tag = 0

    template<typename S>
    void attach( const name_value_pair<S>& nvp, HOLDER_IS_CONVERTIBLE(attribute) )
    {
        typedef serializer_wrapper< attribute, name_value_pair<S> > nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                      nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !attributeSerializers.insert( typename attribute_serializer_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two serializers with same name");
        }
    }

    template<typename S>
    void attach( const name_value_pair<S>& nvp, HOLDER_IS_CONVERTIBLE(element) )
    {
        typedef serializer_wrapper< element, name_value_pair<S> >   nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                      nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !elementSerializers.insert( typename element_serializer_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two serializers with same name");
        }
    }

#undef IS_CONVERTIBLE

private:
    attribute_serializer_map    attributeSerializers;
    element_serializer_map      elementSerializers;
};

template<typename D>
generic_serializer<D>& operator &= (generic_serializer<D>&  gs, 
                                    const nvp_list_end&     /*nvpl*/)
{
    return gs;
}

template<typename D, typename S>
generic_serializer<D>& operator &= (generic_serializer<D>&      gs, 
                                    const name_value_pair<S>&   nvp)
{
    gs.attach(nvp);
    return gs;
}

template<typename D, typename S, typename Rest>
generic_serializer<D>& operator &= (generic_serializer<D>&      gs, 
                                    const nvp_list<S, Rest>&    nvpl)
{
    gs &= nvpl.nvp;
    gs &= nvpl.rest;
    return gs;
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_GENERIC_SERIALIZER_HPP
