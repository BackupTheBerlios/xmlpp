#ifndef XMLPP_SERIALIZATION_HELPERS_HPP
#define XMLPP_SERIALIZATION_HELPERS_HPP

#include <iterator>

// forward
namespace boost 
{
    template<typename T>
    class shared_ptr;
    
    template<typename T>
    class intrusive_ptr;
}

namespace std
{
    template<typename T, typename A>
    class deque;

    template<typename T, typename A>
    class list;

    template<typename T, typename A>
    class vector;
}

namespace xmlpp {

/** Helper class for wrapping type with template.
 * For example, see specialization for boost::shared_ptr:
 * \code
 * template<typename T, typename Y>
 * struct convert_ptr< boost::shared_ptr<T>, Y >
 * {
 *     typedef void                    tag;
 *     typedef boost::shared_ptr<Y>    type;    // Y is wrapped
 * };
 * \uncode
 * Specialize this template as above to enable use of your proprietary smart pointers
 * with ptr serialization functions.
 */
template<typename T, typename Y>
struct convert_ptr;

template<typename T, typename Y>
struct convert_ptr< T*, Y >
{
    typedef void    tag;
    typedef Y*      type;
};

template<typename T, typename Y>
struct convert_ptr< boost::shared_ptr<T>, Y >
{
    typedef void                    tag;
    typedef boost::shared_ptr<Y>    type;
};

template<typename T, typename Y>
struct convert_ptr< boost::intrusive_ptr<T>, Y >
{
    typedef void                    tag;
    typedef boost::intrusive_ptr<Y> type;
};

/** Helper class used to allow functions to work with specified container.
 * If you want allow your container specialize template as follows:
 * \code
 * template<Args>
 * struct is_container< YourContainer<Args> > : public boost::true_type {};
 * \uncode
 * Container must have begin()/end() functions, support iterators and std::back_insert_iterator for
 * deserializing.
 */
template<typename C>
struct is_container : public boost::false_type {};

template<typename T, typename A>
struct is_container< std::vector<T, A> > : public boost::true_type {};

template<typename T, typename A>
struct is_container< std::list<T, A> > : public boost::true_type {};

template<typename T, typename A>
struct is_container< std::deque<T, A> > : public boost::true_type {};

/** Helper class used to allow functions to work with specified iterator.
 * If you want allow work with your iterator specialize template as follows:
 * \code
 * template<YourIterator>
 * struct is_iterator<YourIterator> : public boost::true_type {};
 * \uncode
 */
template<typename Iterator>
class is_iterator
{
private:
	template <typename Signature, Signature> 
	struct TypeCheck;

	typedef char					Yes;
	typedef struct { char foo[2]; }	No;

	template<typename T>
	static Yes has_increment(TypeCheck<T (T::*)(int), &T::operator++>*);

	template<typename T>
	static No  has_increment(...);

public:
	static const bool value = sizeof( has_increment<Iterator>(0) ) == sizeof(Yes);
};

template<typename T>
class is_iterator<T*>
{
public:
	static const bool value = true;
};

template<typename T>
class is_iterator<const T*>
{
public:
	static const bool value = true;
};

#undef HAS_MEM_FUNC

/** Helper class constructing elements of specified type for deserialization(usually pointers).
 * Specialize for your special types if needed.
 */
template<typename T>
struct default_constructor
{
    T operator () (void) { return T(); }
};

template<typename T>
struct default_constructor<T*>
{
    T* operator () (void) { return new T(); }
};

template<typename T>
struct default_constructor< boost::shared_ptr<T> >
{
    boost::shared_ptr<T> operator () (void) { return boost::shared_ptr<T>(new T); }
};

template<typename T>
struct default_constructor< boost::intrusive_ptr<T> >
{
    boost::intrusive_ptr<T> operator () (void) { return boost::intrusive_ptr<T>(new T); }
};

/** Default serialization policy. Calls save/load correspondingly. */
template<typename T, typename Holder = element>
struct default_serialization_policy
{
    typedef Holder xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, T& obj) 
    { 
        obj.load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const T& obj) const
    {
        obj.save(d, e);
    }

    bool valid(const T&) const { return true; }
};

template<typename T, typename Holder>
struct default_serialization_policy<T*, Holder>
{
    typedef Holder xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, T& obj) 
    { 
        assert(obj);
        obj->load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const T& obj) const
    {
        assert(obj);
        obj->save(d, e);
    }

    bool valid(const T* obj) const { return (bool)obj; }
};

template<typename T, typename Holder>
struct default_serialization_policy< boost::shared_ptr<T>, Holder >
{
    typedef Holder xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, boost::shared_ptr<T>& obj) 
    { 
        assert(obj);
        obj->load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const boost::shared_ptr<T>& obj) const
    {
        assert(obj);
        obj->save(d, e);
    }

    bool valid(const boost::shared_ptr<T>& obj) const { return (bool)obj; }
};

template<typename T, typename Holder>
struct default_serialization_policy< boost::intrusive_ptr<T>, Holder >
{
    typedef Holder xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, boost::intrusive_ptr<T>& obj) 
    { 
        assert(obj);
        obj->load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const boost::intrusive_ptr<T>& obj) const
    {
        assert(obj);
        obj->save(d, e);
    }

    bool valid(const boost::intrusive_ptr<T>& obj) const { return (bool)obj; }
};

/** Policy for serializing pointers to objects with dynamic type check. */
template<typename PtrT, typename PtrY>
struct dynamic_ptr_serialization_policy;

template<typename T, typename Y>
struct dynamic_ptr_serialization_policy<T*, Y*>
{
    typedef element xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, T* obj) 
    { 
        static_cast<Y*>(obj)->load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const T* obj) const
    {
        static_cast<Y*>(obj)->save(d, e);
    }

    bool valid(const T* obj) const { return dynamic_cast<Y*>(obj); }
};

template<typename T, typename Y>
struct dynamic_ptr_serialization_policy< boost::shared_ptr<T>, boost::shared_ptr<Y> >
{
    typedef element xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, boost::shared_ptr<T>& obj) 
    { 
        static_cast<Y&>(*obj).load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const boost::shared_ptr<T>& obj) const
    {
        static_cast<Y&>(*obj).save(d, e);
    }

    bool valid(const boost::shared_ptr<T>& obj) const { return (bool)boost::shared_dynamic_cast<Y>(obj); }
};

template<typename T, typename Y>
struct dynamic_ptr_serialization_policy< boost::intrusive_ptr<T>, boost::intrusive_ptr<Y> >
{
    typedef element xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, boost::intrusive_ptr<T>& obj) 
    { 
        static_cast<Y&>(*obj).load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const boost::intrusive_ptr<T>& obj) const
    {
        static_cast<Y&>(*obj).save(d, e);
    }

    bool valid(const boost::intrusive_ptr<T>& obj) const { return dynamic_cast<Y*>(obj.get()); }
};

#pragma region iterator_traits

template<typename Iter, bool IsIterator>
struct iterator_traits_impl
{
private:
	struct unknown_type;

public:
    typedef unknown_type iterator_category;
	typedef unknown_type value_type;
	typedef unknown_type difference_type;
	typedef unknown_type distance_type;
	typedef unknown_type pointer;
	typedef unknown_type reference;
};

template<typename Iter>
struct iterator_traits_impl<Iter, true>
{
    typedef typename std::iterator_traits<Iter>::iterator_category      iterator_category;
	typedef typename std::iterator_traits<Iter>::value_type             value_type;
	typedef typename std::iterator_traits<Iter>::difference_type        difference_type;
	typedef typename std::iterator_traits<Iter>::distance_type          distance_type;
	typedef typename std::iterator_traits<Iter>::pointer                pointer;
	typedef typename std::iterator_traits<Iter>::reference              reference;
};

/** Iterator traits. By default use std::iterator_traits */
template<typename Iter>
struct iterator_traits :
	public iterator_traits_impl<Iter, is_iterator<Iter>::value>
{
};

#pragma endregion iterator_traits

/** class serializer/deserializes object to the element */
template< typename T,
          typename Policy = default_serialization_policy<T>,
          typename Constructor = default_constructor<T> >
class default_loader
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;

public:
    explicit default_loader( T&          obj_, 
                             Constructor cons_ = Constructor(),
                             Policy      policy_ = Policy() ) :
        obj(obj_),
        cons(cons_),
        policy(policy_)
    {}

    /** deserialize item from the element */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) 
    { 
        obj = cons();
        if ( policy.valid(obj) ) {
            policy.load(d, e, obj);
        }
    }
	
	template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    {
		assert(0);
	}

    /** Check for validness */
    bool valid() const { return policy.valid(obj); }

private:
    T&          obj;
    Constructor cons;
    Policy      policy;
};

/** class serializer/deserializes object to the element */
template< typename T, 
          typename Policy = default_serialization_policy<T> >
class default_saver
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;

public:
    explicit default_saver( const T&    obj_, 
                            Policy		policy_ = Policy() ) :
        obj(obj_),
        policy(policy_)
    {}
	
	template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) 
    {
		assert(0);
	}

    /** serialize item into the element */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    { 
        if ( policy.valid(obj) ) {
            policy.save(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid() const { return policy.valid(obj); }

private:
    const T&    obj;
    Policy		policy;
};

/** class serializer/deserializes object to the element */
template< typename T, 
          typename Policy = default_serialization_policy<T>,
          typename Constructor = default_constructor<T> >
class default_serializer
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;

public:
    explicit default_serializer( T&          obj_, 
                                 Constructor cons_ = Constructor(),
                                 Policy      policy_ = Policy() ) :
        obj(obj_),
        cons(cons_),
        policy(policy_)
    {}

    /** deserialize item from the element */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) 
    { 
        obj = cons();
        if ( policy.valid(obj) ) {
            policy.load(d, e, obj);
        }
    }

    /** serialize item into the element */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    { 
        if ( policy.valid(obj) ) {
            policy.save(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid() const { return policy.valid(obj); }

private:
    T&          obj;
    Constructor cons;
    Policy      policy;
};

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_HELPERS_HPP
