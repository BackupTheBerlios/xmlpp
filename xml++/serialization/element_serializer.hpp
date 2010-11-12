#ifndef XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP

#include "../element.h"
#include "helpers.hpp"
#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_trivial_constructor.hpp>
#include <boost/utility/enable_if.hpp>

namespace xmlpp {
	
/** Make object-to-element serializer */
template<typename T>
default_loader
<
	T,
	default_serialization_policy<T>,
	default_constructor<T>
> 
from_element(T& item,
		     typename boost::enable_if< boost::has_trivial_assign<T> >::type* tag0 = 0,
		     typename boost::enable_if< boost::has_trivial_constructor<T> >::type* tag1 = 0 )
{
    typedef default_loader< T,
                            default_serialization_policy<T>,
                            default_constructor<T> > loader;

    return loader(item);
}

/** Make loader of the object from element. */
template<typename T>
default_loader
<
	T,
	default_serialization_policy<T>,
	disable_contructor<T>
> 
from_element(T& item)
{
    typedef default_loader< T,
                            default_serialization_policy<T>,
                            disable_contructor<T> > loader;

    return loader(item);
}

/** Make loader of the object from element. */
template<typename T, typename Constructor>
default_loader
<
    T,
    default_serialization_policy<T>,
    Constructor
> 
from_element(T& item,
             Constructor cons)
{
    typedef default_serializer< T,
                                default_serialization_policy<T>,
                                Constructor > serializer;

    return serializer(item, cons);
}

/** Make loader of the object from element. */
template<typename T, typename Constructor, typename Policy>
default_loader<T, Policy, Constructor> from_element(T& item,
                                                    Constructor cons,
                                                    Policy policy)
{
    return default_loader<T, Policy, Constructor>(item, cons, policy);
}

/** Make saver of the object to element */
template<typename T>
default_saver<T> to_element(T& item)
{
    return default_serializer<T>(item);
}

/** Make object-to-element serializer */
template<typename T>
default_serializer
<
	T,
	default_serialization_policy<T>,
	default_constructor<T>
> 
as_element(T& item,
		   typename boost::enable_if< boost::has_trivial_assign<T> >::type* tag0 = 0,
		   typename boost::enable_if< boost::has_trivial_constructor<T> >::type* tag1 = 0 )
{
    typedef default_serializer< T,
                                default_serialization_policy<T>,
                                default_constructor<T> > serializer;

    return serializer(item);
}

/** Make object-to-element serializer */
template<typename T>
default_serializer
<
	T,
	default_serialization_policy<T>,
	disable_contructor<T>
> 
as_element(T& item)
{
    typedef default_serializer< T,
                                default_serialization_policy<T>,
                                disable_contructor<T> > serializer;

    return serializer(item);
}

/** Make object-to-element serializer */
template<typename T, typename Constructor>
default_serializer
<
    T,
    default_serialization_policy<T>,
    Constructor
> 
from_element(T& item,
             Constructor cons)
{
    typedef default_serializer< T,
                                default_serialization_policy<T>,
                                Constructor > serializer;

    return serializer(item, cons);
}

/** Make object-to-element serializer */
template<typename T, typename Constructor, typename Policy>
default_serializer<T, Policy, Constructor> as_element(T& item,
                                                      Constructor cons,
                                                      Policy policy)
{
    return default_serializer<T, Policy, Constructor>(item, cons, policy);
}

//================================================== DERIVED ==================================================//

/** Serialize object as derived if conversion succeeded. To enable work with proprietary pointers specialize
 * convert_ptr helper template.
 * @tparam Y - derived type.
 * @see convert_ptr, default_serializer
 */
template<typename Y, typename T>
default_serializer
<
    T,
    dynamic_ptr_serialization_policy<T, typename convert_ptr<T, Y>::type>,
    default_constructor<typename convert_ptr<T, Y>::type>
> 
as_element(T& item, 
           typename convert_ptr<T, Y>::tag* toggle = 0)
{
    typedef default_serializer< T, 
                                dynamic_ptr_serialization_policy<T, typename convert_ptr<T, Y>::type>,
                                default_constructor<typename convert_ptr<T, Y>::type> > serializer;

    return serializer(item);
}

/** Serialize object as derived if conversion succeeded. To enable work with proprietary pointers specialize
 * convert_ptr helper template.
 * @tparam Y - derived type.
 * @param cons - constructor for constructing objects for deserialization.
 * @see convert_ptr, default_serializer
 */
template<typename Y, typename T, typename Constructor>
default_serializer
<
    T, 
    dynamic_ptr_serialization_policy<T, typename convert_ptr<T, Y>::type>,
    Constructor
> 
as_element(T& item, 
           Constructor cons,
           typename convert_ptr<T, Y>::tag* toggle = 0)
{
    typedef default_serializer< T, 
                                dynamic_ptr_serialization_policy<T, typename convert_ptr<T, Y>::type>,
                                Constructor > serializer;

    return serializer(item, cons);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ELEMENT_SERIALIZER_HPP
