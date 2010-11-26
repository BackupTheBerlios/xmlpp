#include "serialization/serialization.hpp"

#define BOOST_TEST_MODULE TraitsTest
#include <boost/test/unit_test.hpp>

struct foo
{
	int		value;
	float	otherValue;
};

struct fake_iterator
{
	fake_iterator operator ++ (int) { return *this; }
};

// check iterator_traits
BOOST_AUTO_TEST_CASE(traits_test_0)
{
	using namespace xmlpp;

	BOOST_CHECK_EQUAL( is_iterator<std::vector<int>::iterator>::value, true );
	BOOST_CHECK_EQUAL( is_iterator<std::vector<float>::iterator>::value, true );
	BOOST_CHECK_EQUAL( is_iterator<float*>::value, true );
	BOOST_CHECK_EQUAL( is_iterator< std::back_insert_iterator<std::list<double> > >::value, true );
	BOOST_CHECK_EQUAL( is_iterator<std::vector<float> >::value, false );
	BOOST_CHECK_EQUAL( is_iterator<int>::value, false );
	BOOST_CHECK_EQUAL( is_iterator<float>::value, false );
	BOOST_CHECK_EQUAL( is_iterator<foo>::value, false );

	// in future this test must fail
	BOOST_CHECK_EQUAL( is_iterator<fake_iterator>::value, true );
}

struct loadable { void load(const xmlpp::document& d, const xmlpp::element& e); };
struct savable  { void save(xmlpp::document& d, xmlpp::element& e) const; };

struct serializable :
	public loadable,
	public savable
{    
};

struct virtual_loadable { virtual void load(const xmlpp::document& d, const xmlpp::element& e) = 0; };
struct virtual_savable  { virtual void save(xmlpp::document& d, xmlpp::element& e) const = 0; };

struct virtual_serializable :
	public virtual_loadable,
	public virtual_savable
{    
};

struct template_loadable
{
	template<typename Document, typename Holder>
    void load(const Document& d, const Holder& e);
};

struct template_savable
{
    template<typename Document, typename Holder>
    void save(Document& d, Holder& e) const;
};

struct template_serializable :
	public template_loadable,
	public template_savable
{    
};

// check serializer traits
BOOST_AUTO_TEST_CASE(traits_test_1)
{
	using namespace xmlpp;

	// some stupid tests
	//BOOST_CHECK_EQUAL( (is_serializer<int, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<std::string, document, element>::value), false );

	// check custom serializers
	BOOST_CHECK_EQUAL( (is_loader<loadable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_loader<savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_loader<serializable, document, element>::value), true );

	BOOST_CHECK_EQUAL( (is_saver<loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_saver<savable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_saver<serializable, document, element>::value), true );
	
	BOOST_CHECK_EQUAL( (is_serializer<loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<serializable, document, element>::value), true );

	BOOST_CHECK_EQUAL( (is_loader<loadable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_loader<savable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_loader<serializable, document, attribute>::value), false );

	BOOST_CHECK_EQUAL( (is_saver<loadable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_saver<savable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_saver<serializable, document, attribute>::value), false );
	
	BOOST_CHECK_EQUAL( (is_serializer<loadable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<savable, document, attribute>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<serializable, document, attribute>::value), false );
	
	// check virtual custom serializers
	BOOST_CHECK_EQUAL( (is_loader<virtual_loadable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_loader<virtual_savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_loader<virtual_serializable, document, element>::value), true );

	BOOST_CHECK_EQUAL( (is_saver<virtual_loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_saver<virtual_savable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_saver<virtual_serializable, document, element>::value), true );
	
	BOOST_CHECK_EQUAL( (is_serializer<virtual_loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<virtual_savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<virtual_serializable, document, element>::value), true );

	// check templated custom serializers
	BOOST_CHECK_EQUAL( (is_loader<template_loadable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_loader<template_savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_loader<template_serializable, document, element>::value), true );

	BOOST_CHECK_EQUAL( (is_saver<template_loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_saver<template_savable, document, element>::value), true );
	BOOST_CHECK_EQUAL( (is_saver<template_serializable, document, element>::value), true );
	
	BOOST_CHECK_EQUAL( (is_serializer<template_loadable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<template_savable, document, element>::value), false );
	BOOST_CHECK_EQUAL( (is_serializer<template_serializable, document, element>::value), true );
}
