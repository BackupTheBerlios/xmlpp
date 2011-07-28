#ifndef XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP

#include "element_serializer.hpp"
#include "generic_serializer.hpp"
#include "text_serializer.hpp"

namespace xmlpp {
	
#define ENABLE_IF_CONTAINER(Container)	typename boost::enable_if< is_container<Container> >::type* Container##tag = 0
#define ENABLE_IF_ITERATOR(Iterator)	typename boost::enable_if< is_iterator<Iterator> >::type* Iterator##tag = 0

template< typename OutIterator,
          typename ValueType = typename iterator_traits<OutIterator>::value_type,
          typename Policy = default_serialization_policy<ValueType>,
          typename Constructor = default_constructor<ValueType> >
class container_loader
{
public:
    typedef element     xmlpp_holder_type;
    typedef Constructor constructor_type;

public:
    container_loader( OutIterator outIter_, 
                      Constructor constructor_ = Constructor(),
                      Policy      policy_ = Policy() ) :
        outIter(outIter_),
        constructor(constructor_),
        policy(policy_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        ValueType value = constructor();
        if ( policy.valid(value, LOAD) ) {
            policy.load(d, e, value);
        }
        (*outIter++) = value;
    }

public:
    OutIterator outIter;
    Constructor constructor;
    Policy      policy;
};

template<typename InIterator, 
         typename ValueType = typename iterator_traits<InIterator>::value_type,
         typename Policy = default_serialization_policy<ValueType> >
class container_saver
{
public:
    typedef element xmlpp_holder_type;

public:
    container_saver(InIterator firstIter_, 
                    InIterator endIter_,
                    Policy     policy_ = Policy() ) :
        firstIter(firstIter_),
        endIter(endIter_),
        policy(policy_)
    {}
	
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        xmlpp::element_iterator holder(e);
        for (InIterator iter  = firstIter;
                        iter != endIter;
                        ++iter)
        {
            if ( policy.valid(*iter, SAVE) ) {
                policy.save(d, *holder++, *iter);
            }
        }
    }

public:
    InIterator  firstIter;
    InIterator  endIter;
    Policy      policy;
};

template<typename InIterator, 
         typename OutIterator, 
         typename ValueType,
         typename Policy = default_serialization_policy<ValueType>,
         typename Constructor = default_constructor<ValueType> >
class container_serializer
{
public:
    typedef element     xmlpp_holder_type;
    typedef Constructor constructor_type;

public:
    container_serializer( InIterator    firstIter_,
                          InIterator    endIter_,
                          OutIterator   outIter_,
                          Constructor   constructor_ = Constructor(),
                          Policy        policy_ = Policy() ) :
        firstIter(firstIter_),
        endIter(endIter_),
        outIter(outIter_),
        constructor(constructor_),
        policy(policy_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        ValueType value = constructor();
        if ( policy.valid(value, LOAD) ) {
            policy.load(d, e, value);
        }
        (*outIter++) = value;
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        xmlpp::element_iterator holder(e);
        for (InIterator iter  = firstIter;
                        iter != endIter;
                        ++iter)
        {
            if ( policy.valid(*iter, SAVE) ) {
                policy.save(d, *holder++, *iter);
            }
        }
    }

public:
    InIterator  firstIter;
    InIterator  endIter;
    OutIterator outIter;
    Constructor constructor;
    Policy      policy;
};

template< typename InIterator, 
          typename Policy >
struct generic_holder< name_value_pair< container_saver<InIterator, 
                                                        Policy> >, 
                       xmlpp::element >
{
    typedef name_value_pair< container_saver<InIterator, 
                                             Policy> > nvp_type;

    element operator () (const nvp_type& nvp, node& parent)
    {
        if ( nvp.serializer.firstIter != nvp.serializer.endIter )
        {
            InIterator iter = nvp.serializer.firstIter;

            // allocate children for every object in the sequence
            element first;
            for (; iter != nvp.serializer.endIter; ++iter)
            {
                element child(nvp.name);
                if ( nvp.serializer.policy.valid(*iter, SAVE) ) 
				{
                    add_child(parent, child);
					if ( !first.get_tixml_node() ) {
						first = child;
					}
                }
            }

            return first;
        }

        return element("null");
    }
};

template<typename InIterator, 
         typename OutIterator, 
         typename ValueType,
         typename Policy,
         typename Constructor>
struct generic_holder< name_value_pair< container_serializer<InIterator, 
                                                             OutIterator,
                                                             ValueType,
                                                             Policy,
                                                             Constructor> >, 
                       xmlpp::element >
{
    typedef name_value_pair< container_serializer<InIterator, 
                                                  OutIterator,
                                                  ValueType,
                                                  Policy,
                                                  Constructor> > nvp_type;

    element operator () (const nvp_type& nvp, node& parent)
    {
        if ( nvp.serializer.firstIter != nvp.serializer.endIter )
        {
            InIterator iter = nvp.serializer.firstIter;

            // allocate children for every object in the sequence
            element first;
            for (; iter != nvp.serializer.endIter; ++iter)
            {
                element child(nvp.name);
                if ( nvp.serializer.policy.valid(*iter, SAVE) ) 
				{
                    add_child(parent, child);
					if ( !first.get_tixml_node() ) {
						first = child;
					}
				}
            }

            return first;
        }

        return element();
    }
};

// container to text string
template<typename InIterator>
class container_to_string
{
public:
    typedef element xmlpp_holder_type;

public:
    container_to_string(InIterator begin_, InIterator end_) :
        begin(begin_),
        end(end_)
    {}

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        std::ostringstream ss;
        for (InIterator iter  = begin;
                        iter != end;
                        ++iter)
        {
            ss << (*iter);
        }
        e.set_text( ss.str().c_str() );
    }

private:
    InIterator  begin;
    InIterator  end;
};

template< typename OutIterator,
          typename ValueType,
          typename Constructor = default_constructor<ValueType> >
class container_from_string
{
public:
    typedef element xmlpp_holder_type;

public:
    explicit container_from_string( OutIterator out_,
                                    Constructor constructor_ = Constructor() ) :
        out(out_),
        constructor(constructor_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
		if ( !e.get_text() || e.get_text()[0] == '\0' ) {
			return;
		}

        std::istringstream ss( e.get_text() );
        while ( !ss.eof() ) 
        {
            ValueType val = constructor();
            ss >> val;
            if ( ss.fail() ) {
                throw dom_error("Can't read element value");
            }
            *out++ = val;
        }
    }

private:
    OutIterator out;
    Constructor constructor;
};

template< typename InIterator, 
          typename OutIterator,
          typename ValueType,
          typename Constructor = default_constructor<ValueType> >
class container_as_string :
    public container_to_string<InIterator>,
    public container_from_string<OutIterator, ValueType, Constructor>
{
public:
    typedef element xmlpp_holder_type;

public:
    container_as_string( InIterator begin, 
                         InIterator end, 
                         OutIterator out,
                         Constructor constructor = Constructor() ) :
        container_to_string<InIterator>(begin, end),
        container_from_string<OutIterator, ValueType, Constructor>(out, constructor)
    {}
};

//================================================== STRING ==================================================//

/** Make saver, printing range of elements to the string using std::ostringstream::operator << 
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename InIterator>
container_to_string<InIterator> to_string( InIterator begin,
										   InIterator end,
										   ENABLE_IF_ITERATOR(InIterator) )
{
    return container_to_string<InIterator>(begin, end);
}

/** Make saver, printing container elements to the string using std::ostringstream::operator << 
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_to_string<typename Container::const_iterator> to_string( const Container& values, 
                                                                   ENABLE_IF_CONTAINER(Container) )
{
    return container_to_string<typename Container::const_iterator>( values.begin(), values.end() );
}

/** Make loader, reading from the string using std::istringstream::operator >>
 * @param out - output iterator for placing readed elements.
 */ 
template<typename OutIterator>
container_from_string
<
    OutIterator, 
    typename iterator_traits<OutIterator>::value_type
> 
from_string_it( OutIterator out,
				ENABLE_IF_ITERATOR(OutIterator) )
{
    return container_from_string<OutIterator, typename iterator_traits<OutIterator>::value_type>(out);
}

/** Make loader, reading from the string using std::istringstream::operator >>
 * @param out - output iterator for placing readed elements.
 * @param cons - functor for constructing container objects.
 */ 
template<typename Constructor, typename OutIterator>
container_from_string
<
    OutIterator, 
    typename iterator_traits<OutIterator>::value_type, 
    Constructor
> 
from_string( OutIterator out, 
			 Constructor cons,
			 ENABLE_IF_ITERATOR(OutIterator) )
{
    return container_from_string<OutIterator, typename iterator_traits<OutIterator>::value_type, Constructor>(out, cons);
}

/** Make loader, reading from the string and adding them to the end of the container using std::istringstream::operator >>
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_from_string
< 
    std::back_insert_iterator<Container>, 
    typename Container::value_type 
> 
from_string( Container& values, 
             ENABLE_IF_CONTAINER(Container) )
{
    return container_from_string< std::back_insert_iterator<Container>, typename Container::value_type >( std::back_inserter(values) );
}

/** Make loader, reading from the string and adding them to the end of the container using std::istringstream::operator >>
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Constructor, typename Container>
container_from_string
< 
    std::back_insert_iterator<Container>, 
    typename Container::value_type,
    Constructor
> 
from_string( Container& values, 
             Constructor cons, 
             ENABLE_IF_CONTAINER(Container) )
{
    typedef container_from_string< std::back_insert_iterator<Container>, 
                                   typename Container::value_type,
                                   Constructor > serializer;

    return serializer( std::back_inserter(values), cons );
}

/** Make serializaer, saving elements in the range to the string using std::ostringstream::operator << and reading
 * elements from the string, using std::istringstream::operator >> 
 * @param begin - iterator addressing first element in the range for saving.
 * @param end - iterator addressing end element in the range for saving.
 * @param out - iterator for placing read elements.
 */
template<typename InIterator,
         typename OutIterator>
container_as_string
<
    InIterator, 
    OutIterator, 
    typename iterator_traits<OutIterator>::value_type
> 
as_string( InIterator begin, 
		   InIterator end, 
		   OutIterator out,
		   ENABLE_IF_ITERATOR(InIterator),
		   ENABLE_IF_ITERATOR(OutIterator) )
{
    return container_as_string<InIterator, OutIterator, typename iterator_traits<OutIterator>::value_type>(begin, end, out);
}

/** Make serializaer, saving elements in the range to the string using std::ostringstream::operator << and reading
 * elements from the string, using std::istringstream::operator >> 
 * @param begin - iterator addressing first element in the range for saving.
 * @param end - iterator addressing end element in the range for saving.
 * @param out - iterator for placing read elements.
 * @param const - functor constructing objects for output.
 */
template<typename Constructor,
         typename InIterator,
         typename OutIterator>
container_as_string
<
    InIterator, 
    OutIterator, 
    typename iterator_traits<OutIterator>::value_type, 
    Constructor
> 
as_string( InIterator begin, 
		   InIterator end, 
		   OutIterator out, 
		   Constructor cons,
		   ENABLE_IF_ITERATOR(InIterator),
		   ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_as_string< InIterator, 
                                 OutIterator, 
                                 typename iterator_traits<OutIterator>::value_type, 
                                 Constructor > serializer;

    return serializer(begin, end, out, cons);
}

/** Make serializer, saving and loading container elements to/from string using std::stringstream::operator <</>>
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_as_string
< 
    typename Container::const_iterator, 
    std::back_insert_iterator<Container>, 
    typename Container::value_type 
> 
as_string( Container& values,
           ENABLE_IF_CONTAINER(Container) )
{
    typedef container_as_string < typename Container::const_iterator, 
                                  std::back_insert_iterator<Container>, 
                                  typename Container::value_type > serializer;

    return serializer( values.begin(), values.end(), std::back_inserter(values) );
}
//================================================== CUSTOM ==================================================//

template<typename OutIterator, typename Policy>
container_loader
< 
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    Policy
>
from_element_set_ex( OutIterator out,
                     Policy      policy,
				     ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_loader< OutIterator,
                              typename iterator_traits<OutIterator>::value_type,
                              Policy > serializer;

    return serializer(out, serializer::constructor_type(), policy);
}

/** Make loader, loading container elements from corresponding xml elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container, typename Policy>
container_loader
< 
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    Policy
>
from_element_set_ex( Container& values,
                     Policy     policy,
                     ENABLE_IF_CONTAINER(Container) )
{
    typedef container_loader< std::back_insert_iterator<Container>,
                              typename Container::value_type,
                              Policy > serializer;

    return serializer( std::back_inserter(values), serializer::constructor_type(), policy );
}

template<typename InIterator, typename Policy>
container_saver
< 
    InIterator,  
	typename iterator_traits<InIterator>::value_type,
    Policy
>
to_element_set_ex( InIterator begin, 
                   InIterator end,
                   Policy policy,
			       ENABLE_IF_ITERATOR(InIterator) )
{
    typedef container_saver< InIterator,
                             Policy > serializer;

    return serializer(begin, end, policy);
}

/** Make saver, saving container elements to corresponding xml elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container, typename Policy>
container_saver
< 
    typename Container::const_iterator,
	typename Container::value_type,
    Policy
>
to_element_set_ex( const Container& values,
                   Policy           policy,
                   ENABLE_IF_CONTAINER(Container) )
{
    typedef container_saver< typename Container::const_iterator,
							 typename Container::value_type,
                             Policy > serializer;

    return serializer(values.begin(), values.end(), policy);
}

template<typename InIterator, typename OutIterator, typename Policy>
container_serializer
<
    InIterator,
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    Policy
>
as_element_set_ex( InIterator begin, 
				   InIterator end, 
				   OutIterator out,
                   Policy policy,
				   ENABLE_IF_ITERATOR(InIterator),
				   ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_serializer< InIterator,
                                  OutIterator,
                                  typename iterator_traits<OutIterator>::value_type,
                                  Policy > serializer;

    return serializer(begin, end, out, serializer::contructor_type(), policy);
}

/** Make serializer, saving and loading container elements to/from corresponding elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container, typename Policy>
container_serializer
< 
    typename Container::iterator,
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    Policy
>
as_element_set_ex( Container& values,
                   Policy policy,
                   ENABLE_IF_CONTAINER(Container) )
{
    typedef container_serializer< typename Container::iterator,
								  std::back_insert_iterator<Container>,
								  typename Container::value_type,
								  Policy > serializer;

    return serializer(values.begin(), values.end(), std::back_inserter(values), serializer::constructor_type(), policy);
}

//================================================== ELEMENT ==================================================//

template<typename OutIterator>
container_loader
< 
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    default_serialization_policy<typename iterator_traits<OutIterator>::value_type> 
>
from_element_set( OutIterator out,
				  ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_loader< OutIterator,
                              typename iterator_traits<OutIterator>::value_type,
                              default_serialization_policy<typename iterator_traits<OutIterator>::value_type>  > serializer;

    return serializer(out);
}

/** Make loader, loading container elements from corresponding xml elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_loader
< 
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    default_serialization_policy<typename Container::value_type> 
>
from_element_set( Container& values,
                  ENABLE_IF_CONTAINER(Container) )
{
    typedef container_loader< std::back_insert_iterator<Container>,
                              typename Container::value_type,
                              default_serialization_policy<typename Container::value_type> > serializer;

    return serializer( std::back_inserter(values) );
}

template<typename InIterator>
container_saver
< 
    InIterator,  
	typename iterator_traits<InIterator>::value_type,
    default_serialization_policy<typename iterator_traits<InIterator>::value_type> 
>
to_element_set( InIterator begin, 
                InIterator end,
			    ENABLE_IF_ITERATOR(InIterator) )
{
    typedef container_saver< InIterator,
                             default_serialization_policy<typename iterator_traits<InIterator>::value_type> > serializer;

    return serializer(begin, end);
}

/** Make saver, saving container elements to corresponding xml elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_saver
< 
    typename Container::const_iterator,
	typename Container::value_type,
    default_serialization_policy<typename Container::value_type> 
>
to_element_set( const Container& values,
                ENABLE_IF_CONTAINER(Container) )
{
    typedef container_saver< typename Container::const_iterator,
							 typename Container::value_type,
                             default_serialization_policy<typename Container::value_type> > serializer;

    return serializer( values.begin(), values.end() );
}

template<typename InIterator, typename OutIterator>
container_serializer
<
    InIterator,
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    default_serialization_policy<typename iterator_traits<OutIterator>::value_type> 
>
as_element_set( InIterator begin, 
				InIterator end, 
				OutIterator out,
				ENABLE_IF_ITERATOR(InIterator),
				ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_serializer< InIterator,
                                  OutIterator,
                                  typename iterator_traits<OutIterator>::value_type,
                                  default_serialization_policy<typename iterator_traits<OutIterator>::value_type> > serializer;

    return serializer(begin, end, out);
}

/** Make serializer, saving and loading container elements to/from corresponding elements.
 * @tparam Container - container type. By default enabled for std::vector, std::list, std::deque. 
 * @see enable_for_container
 */ 
template<typename Container>
container_serializer
< 
    typename Container::iterator,
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    default_serialization_policy<typename Container::value_type> 
>
as_element_set( Container& values,
                ENABLE_IF_CONTAINER(Container) )
{
    typedef container_serializer< typename Container::iterator,
								  std::back_insert_iterator<Container>,
								  typename Container::value_type,
								  default_serialization_policy<typename Container::value_type> > serializer;

    return serializer( values.begin(), values.end(), std::back_inserter(values) );
}

//================================================== TEXT ==================================================//

template<typename OutIterator>
container_loader
< 
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    text_serialization_policy<typename iterator_traits<OutIterator>::value_type> 
>
from_text_set( OutIterator out,
			   ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_loader< OutIterator,
                              typename iterator_traits<OutIterator>::value_type,
                              text_serialization_policy<typename iterator_traits<OutIterator>::value_type> > serializer;

    return serializer(out);
}

template<typename InIterator>
container_saver
<
    InIterator,
    text_serialization_policy<typename iterator_traits<InIterator>::value_type> 
>
to_text_set( InIterator begin, 
			 InIterator end,
			 ENABLE_IF_ITERATOR(InIterator) )
{
    typedef container_saver< InIterator,
                             text_serialization_policy<typename InIterator::value_type> > serializer;

    return serializer(begin, end);
}

template<typename InIterator, typename OutIterator>
container_serializer
< 
    InIterator,
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    text_serialization_policy<typename iterator_traits<OutIterator>::value_type> 
>
as_text_set( InIterator begin, 
			 InIterator end, 
			 OutIterator out,
			 ENABLE_IF_ITERATOR(InIterator),
			 ENABLE_IF_ITERATOR(OutIterator) )
{
    typedef container_serializer< InIterator,
                                  OutIterator,
                                  typename iterator_traits<OutIterator>::value_type,
                                  text_serialization_policy<typename iterator_traits<OutIterator>::value_type> > serializer;

    return serializer(begin, end, out);
}

template<typename Container>
container_serializer
< 
    typename Container::iterator,
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    text_serialization_policy<typename Container::value_type> 
>
as_text_set(Container& values,
            ENABLE_IF_CONTAINER(Container))
{
    typedef container_serializer< typename Container::iterator,
                                  std::back_insert_iterator<Container>,
                                  typename Container::value_type,
                                  text_serialization_policy<typename Container::value_type> > serializer;

    return serializer( values.begin(), values.end(), std::back_inserter(values) );
}

//================================================== DERIVED ==================================================//

template<typename Y, typename OutIterator>
container_loader
<
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    dynamic_ptr_serialization_policy<typename iterator_traits<OutIterator>::value_type, typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>,
    default_constructor<typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>
>
from_element_set(OutIterator out,
                 typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::tag* toggle = 0,
			     ENABLE_IF_ITERATOR(OutIterator))
{
    typedef container_loader< OutIterator,
                              typename iterator_traits<OutIterator>::value_type,
                              dynamic_ptr_serialization_policy<typename iterator_traits<OutIterator>::value_type, typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>,
                              default_constructor<typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>
                             > serializer;

    return serializer(out);
}

template<typename Y, typename InIterator>
container_saver
< 
    InIterator,
    dynamic_ptr_serialization_policy<typename iterator_traits<InIterator>::value_type, typename convert_ptr<typename iterator_traits<InIterator>::value_type, Y>::type>
>
to_element_set(InIterator begin, 
               InIterator end,
               typename convert_ptr<typename iterator_traits<InIterator>::value_type, Y>::tag* toggle = 0,
			   ENABLE_IF_ITERATOR(InIterator))
{
    typedef container_saver< InIterator,
                             dynamic_ptr_serialization_policy<typename iterator_traits<InIterator>::value_type, typename convert_ptr<typename iterator_traits<InIterator>::value_type, Y>::type>
                           > serializer;

    return serializer(begin, end);
}

template<typename Y, typename InIterator, typename OutIterator>
container_serializer
< 
    InIterator,
    OutIterator,
    typename iterator_traits<OutIterator>::value_type,
    dynamic_ptr_serialization_policy<typename iterator_traits<OutIterator>::value_type, typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>,
    default_constructor<typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>
>
as_element_set(InIterator begin, 
               InIterator end, 
               OutIterator out,
               typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::tag* toggle = 0,
			   ENABLE_IF_ITERATOR(InIterator),
			   ENABLE_IF_ITERATOR(OutIterator))
{
    typedef container_serializer< InIterator,
                                  OutIterator,
                                  typename iterator_traits<OutIterator>::value_type,
                                  dynamic_ptr_serialization_policy<typename iterator_traits<OutIterator>::value_type, typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>,
                                  default_constructor<typename convert_ptr<typename iterator_traits<OutIterator>::value_type, Y>::type>
                                 > serializer;

    return serializer(begin, end, out);
}

template<typename Y, typename Container>
container_saver
< 
    typename Container::const_iterator,
    dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>
>
to_element_set(const Container& values,
               ENABLE_IF_CONTAINER(Container),
               typename convert_ptr<typename Container::value_type, Y>::tag* tag1 = 0)
{
    typedef container_saver< typename Container::const_iterator,
                             dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>
                           > serializer;

    return serializer( values.begin(), values.end() );
}

template<typename Y, typename Container>
container_loader
< 
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>,
    default_constructor< typename convert_ptr<typename Container::value_type, Y>::type >
>
from_element_set(Container& values,
                 ENABLE_IF_CONTAINER(Container),
                 typename convert_ptr<typename Container::value_type, Y>::tag* tag1 = 0)
{
    typedef container_loader< std::back_insert_iterator<Container>,
                              typename Container::value_type,
                              dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>,
                              default_constructor< typename convert_ptr<typename Container::value_type, Y>::type >
                             > serializer;

    return serializer( std::back_inserter(values) );
}

template<typename Y, typename Container>
container_serializer
< 
    typename Container::iterator,
    std::back_insert_iterator<Container>,
    typename Container::value_type,
    dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>,
    default_constructor< typename convert_ptr<typename Container::value_type, Y>::type >
>
as_element_set(Container& values,
               ENABLE_IF_CONTAINER(Container),
               typename convert_ptr<typename Container::value_type, Y>::tag* tag1 = 0)
{
    typedef container_serializer< typename Container::iterator,
								  std::back_insert_iterator<Container>,
								  typename Container::value_type,
								  dynamic_ptr_serialization_policy<typename Container::value_type, typename convert_ptr<typename Container::value_type, Y>::type>,
								  default_constructor< typename convert_ptr<typename Container::value_type, Y>::type >
                                 > serializer;

    return serializer( values.begin(), values.end(), std::back_inserter(values) );
}

#undef ENABLE_IF_CONTAINER
#undef ENABLE_IF_ITERATOR

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
