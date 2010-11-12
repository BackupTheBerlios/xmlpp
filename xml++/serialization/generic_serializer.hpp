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
#define XMLPP_SERIALIZATION_SPLIT_MEMBER(Document, Holder) \
    void serialize(Document& d, Holder& n, xmlpp::s_state state)\
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

/** Trait class to determine whether serializer class has load function */
HAS_XXX(load)
template<typename T, typename Document, typename Holder>
class is_loader :
	public is_load_call_possible<T, void (Document, Holder)>
{
};

/** Trait class to determine whether serializer class has save function */
HAS_XXX(save)
template<typename T, typename Document, typename Holder>
class is_saver :
	public is_save_call_possible<T, void (Document, Holder)>
{
};

/** Trait class to determine whether serializer class has load & save function */
template<typename T, typename Document, typename Holder>
class is_serializer
{
public:
	static const bool value = (is_loader<T, Document, Holder>::value && is_saver<T, Document, Holder>::value);
};

/** Generated loader. Main class for constructing parsers of the xml chunks.
 * @tparam Document - type of the document for serialization, generally user specified or xmlpp::document.
 */
template<typename Document>
class generic_loader
{
public:
    typedef element xmlpp_holder_type;

private:
    template<typename Holder>
    class loader
    {
    public:
        virtual void load(const Document&, const Holder&) = 0;

        virtual ~loader() {}
    };

    template<typename Holder, typename Loader>
    class loader_wrapper :
        public loader<Holder>
    {
    public:
        loader_wrapper(const Loader& loader_) :
            loader(loader_)
        {}

        void load(const Document& d, const Holder& n) 
		{ 
			loader.load(d, n); 
		}

    public:
        Loader loader;
    };

private:
    typedef boost::shared_ptr< loader<element> >			element_loader_ptr;
    typedef std::map<std::string, element_loader_ptr>       element_loader_map;
    typedef typename element_loader_map::iterator           element_loader_iterator;
    typedef typename element_loader_map::const_iterator     element_loader_const_iterator;

    typedef boost::shared_ptr< loader<attribute> >			attribute_loader_ptr;
    typedef std::map<std::string, attribute_loader_ptr>     attribute_loader_map;
    typedef typename attribute_loader_map::iterator         attribute_loader_iterator;
    typedef typename attribute_loader_map::const_iterator   attribute_loader_const_iterator;

public:
    /** Load from the document, ignore attributes */
    void load(const Document& d)   
    {
        // read elements
        for( const_element_iterator i  = d.first_child_element();
                                    i != d.end_child_element();
                                    ++i )
        {
            element_loader_iterator j = elementLoaders.find( i->get_value() );
            if ( j != elementLoaders.end() ) {
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
            attribute_loader_iterator j = attributeLoaders.find( i->get_name() );
            if ( j != attributeLoaders.end() ) {
				j->second->load(d, *i);
            }
        }

        // read elements
        for( const_element_iterator i  = e.first_child_element();
                                    i != e.end_child_element();
                                    ++i )
        {
            element_loader_iterator j = elementLoaders.find( i->get_value() );
            if ( j != elementLoaders.end() ) {
				j->second->load(d, *i);
            }
        }
    }

	void clear()
	{
		attributeLoaders.clear();
		elementLoaders.clear();
	}

    XMLPP_SERIALIZATION_SPLIT_MEMBER(Document, element)

#define IS_LOADABLE_FROM(From) typename boost::enable_if< is_loader<S, Document, From> >::type* tag = 0

    template<typename S>
    void attach_loader( const name_value_pair<S>& nvp, IS_LOADABLE_FROM(attribute) )
    {
        typedef loader_wrapper< attribute, name_value_pair<S> > nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                  nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !attributeLoaders.insert( typename attribute_loader_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two serializers with same name");
        }
    }

    template<typename S>
    void attach_loader( const name_value_pair<S>& nvp, IS_LOADABLE_FROM(element) )
    {
        typedef loader_wrapper< element, name_value_pair<S> > nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !elementLoaders.insert( typename element_loader_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two serializers with same name");
        }
    }

#undef IS_LOADABLE_FROM

	generic_loader& operator >>= (const nvp_list_end&  /*nvpl*/)
	{
		return (*this);
	}

	template<typename S>
	generic_loader& operator >>= (const name_value_pair<S>&   nvp)
	{
		attach_loader(nvp);
		return (*this);
	}

	template<typename S, typename Rest>
	generic_loader& operator >>= (const nvp_list<S, Rest>&   nvpl)
	{
		(*this) &= nvpl.nvp;
		(*this) &= nvpl.rest;
		return (*this);
	}

private:
    attribute_loader_map    attributeLoaders;
    element_loader_map		elementLoaders;
};

/** Generated saver. Main class for constructing xml structure savers.
 * @tparam Document - type of the document for serialization, generally user specified or xmlpp::document.
 */
template<typename Document>
class generic_saver
{
public:
    typedef element xmlpp_holder_type;

private:
    template<typename Holder>
    class saver
    {
    public:
        virtual void save(Document&, xmlpp::node&) const = 0;

        virtual ~saver() {}
    };

    template<typename Holder, typename Saver>
    class saver_wrapper :
        public saver<Holder>
    {
    public:
        saver_wrapper(const Saver& saver_) :
            saver(saver_)
        {}

        void save(Document& d, xmlpp::node& parent) const       
        {
            generic_holder<Saver, Holder> holder;
            Holder h = holder(saver, parent);
            saver.save(d, h);
        }

    public:
        Saver saver;
    };

private:
    typedef boost::shared_ptr< saver<element> >				element_saver_ptr;
    typedef std::map<std::string, element_saver_ptr>		element_saver_map;
    typedef typename element_saver_map::iterator			element_saver_iterator;
    typedef typename element_saver_map::const_iterator		element_saver_const_iterator;

    typedef boost::shared_ptr< saver<attribute> >			attribute_saver_ptr;
    typedef std::map<std::string, attribute_saver_ptr>		attribute_saver_map;
    typedef typename attribute_saver_map::iterator			attribute_saver_iterator;
    typedef typename attribute_saver_map::const_iterator	attribute_saver_const_iterator;

public:
    /** Save to the document, ignore attributes */
    void save(Document& d) const   
    {
        // save elements
        for( element_saver_const_iterator i  = elementSavers.begin();
                                          i != elementSavers.end();
                                          ++i )
        {
            i->second->save(d, d);
        }
    }

    /** Save to the element. */
    void save(Document& d, xmlpp_holder_type& e) const   
    {
        // save attributes
        for( attribute_saver_const_iterator i  = attributeSavers.begin();
                                            i != attributeSavers.end();
                                            ++i )
        {
            i->second->save(d, e);
        }

        // save elements
        for( element_saver_const_iterator i  = elementSavers.begin();
                                          i != elementSavers.end();
                                          ++i )
        {
            i->second->save(d, e);
        }
    }
	
	void clear()
	{
		attributeSavers.clear();
		elementSavers.clear();
	}

    XMLPP_SERIALIZATION_SPLIT_MEMBER(Document, element)

#define IS_STORABLE_INTO(To) typename boost::enable_if< is_saver<S, Document, To> >::type* tag = 0

    template<typename S>
    void attach_saver( const name_value_pair<S>& nvp, IS_STORABLE_INTO(attribute) )
    {
        typedef saver_wrapper< attribute, name_value_pair<S> > nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                 nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !attributeSavers.insert( typename attribute_saver_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two savers with same name");
        }
    }

    template<typename S>
    void attach_saver( const name_value_pair<S>& nvp, IS_STORABLE_INTO(element) )
    {
        typedef saver_wrapper< element, name_value_pair<S> >   nvp_wrapper;
        typedef boost::shared_ptr<nvp_wrapper>                 nvp_wrapper_ptr;
        
        nvp_wrapper_ptr nvpClone( new nvp_wrapper(nvp) );
        if ( !elementSavers.insert( typename element_saver_map::value_type(nvp.name, nvpClone) ).second ) {
            throw std::logic_error("Can't insert two savers with same name");
        }
    }

#undef IS_STORABLE_INTO

	generic_saver& operator <<= (const nvp_list_end&  /*nvpl*/)
	{
		return (*this);
	}

	template<typename S>
	generic_saver& operator <<= (const name_value_pair<S>&   nvp)
	{
		attach_saver(nvp);
		return (*this);
	}

	template<typename S, typename Rest>
	generic_saver& operator <<= (const nvp_list<S, Rest>&   nvpl)
	{
		(*this) &= nvpl.nvp;
		(*this) &= nvpl.rest;
		return (*this);
	}

private:
    attribute_saver_map    attributeSavers;
    element_saver_map      elementSavers;
};

/** Generated loader & saver. Main class for constructing parsers of the xml chunks.
 * @tparam Document - type of the document for serialization, generally user specified or xmlpp::document.
 */
template<typename Document>
class generic_serializer :
	public generic_loader<Document>,
	public generic_saver<Document>
{
public:
    typedef element xmlpp_holder_type;

public:
    XMLPP_SERIALIZATION_SPLIT_MEMBER(Document, element)

	void clear()
	{
		generic_loader<Document>::clear();
		generic_saver<Document>::clear();
	}

#define IS_SERIALIZABLE(ToFrom) typename boost::enable_if< is_serializer<S, Document, ToFrom> >::type* tag = 0

    template<typename S>
    void attach( const name_value_pair<S>& nvp, IS_SERIALIZABLE(attribute) )
    {
        attach_loader(nvp);
        attach_saver(nvp);
    }

    template<typename S>
    void attach( const name_value_pair<S>& nvp, IS_SERIALIZABLE(element) )
    {
        attach_loader(nvp);
        attach_saver(nvp);
    }

#undef IS_SERIALIZABLE

	generic_serializer& operator &= (const nvp_list_end&  /*nvpl*/)
	{
		return (*this);
	}

	template<typename S>
	generic_serializer& operator &= (const name_value_pair<S>&   nvp)
	{
		attach(nvp);
		return (*this);
	}

	template<typename S, typename Rest>
	generic_serializer& operator &= (const nvp_list<S, Rest>&   nvpl)
	{
		(*this) &= nvpl.nvp;
		(*this) &= nvpl.rest;
		return (*this);
	}
};

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_GENERIC_SERIALIZER_HPP
