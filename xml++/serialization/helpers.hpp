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

/** Serialization state*/
enum s_state
{
    LOAD,
    SAVE
};

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

namespace details
{
    template <typename type>
    class void_exp_result {};

    template <typename type, typename U>
    U const& operator,(U const&, void_exp_result<type>);

    template <typename type, typename U>
    U& operator,(U&, void_exp_result<type>);

    template <typename src_type, typename dest_type>
    struct clone_constness
    {    
        typedef dest_type type;
    };

    template <typename src_type, typename dest_type>
    struct clone_constness<const src_type, dest_type>
    {
        typedef const dest_type type;
    };
}

#define HAS_XXX(member)\
template <typename Type>\
class has_##member\
{\
    class yes	{ char m; };\
    class no	{ yes  m[2]; };\
\
    template <typename Signature, Signature> class test{};\
\
    struct base_xxx { void member(); };\
    struct base : public Type, public base_xxx {};\
\
    template <typename U>\
    static no  deduce(U*, test<void (base_xxx::*)(), &U::member>* = 0);\
    static yes deduce(...);\
\
public:\
    static const bool result = (sizeof(deduce((base*)0)) == sizeof(yes));\
};\
\
template <typename type, typename call_details>\
struct is_##member##_call_possible\
{\
private:\
    class yes	{ char m; };\
    class no	{ yes  m[2]; };\
\
    struct derived : public type\
    {\
        using type::member;\
        no member(...) const;\
    };\
\
    typedef typename details::clone_constness<type, derived>::type derived_type;\
\
    template <typename T, typename due_type>\
    struct return_value_check\
    {\
        static yes deduce(due_type);\
        static no  deduce(...);\
        static no  deduce(no);\
        static no  deduce(details::void_exp_result<type>);\
    };\
\
    template <typename T>\
    struct return_value_check<T, void>\
    {\
        static yes deduce(...);\
        static no  deduce(no);\
    };\
\
    template <bool has, typename F>\
    struct impl\
    {\
        static const bool value = false;\
    };\
\
    template <typename arg1, typename r>\
    struct impl<true, r(arg1)>\
    {\
        static const bool value =\
            sizeof(return_value_check<type, r>::deduce( (((derived_type*)0)->member(*(arg1*)0), details::void_exp_result<type>()))) == sizeof(yes);\
    };\
\
    template <typename arg1, typename arg2, typename r>\
    struct impl<true, r(arg1, arg2)>\
    {\
        static const bool value =\
            sizeof(return_value_check<type, r>::deduce( (((derived_type*)0)->member(*(arg1*)0, *(arg2*)0), details::void_exp_result<type>()))) == sizeof(yes);\
    };\
\
public:\
    static const bool value = impl<has_##member <type>::result, call_details>::value;\
};

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


/** Helper constructor which does nothing */
template<typename T>
struct disable_contructor {};

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

    bool valid(const T&, xmlpp::s_state) const { return true; }
};

template<typename T, typename Holder>
struct default_serialization_policy<T*, Holder>
{
    typedef Holder xmlpp_holder_type;

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e, T*& obj) 
    { 
        assert(obj);
        obj->load(d, e);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e, const T*& obj) const
    {
        assert(obj);
        obj->save(d, e);
    }

    bool valid(const T* obj, s_state) const { return (bool)obj; }
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

    bool valid(const boost::shared_ptr<T>& obj, s_state) const { return (bool)obj; }
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

    bool valid(const boost::intrusive_ptr<T>& obj, s_state) const { return (bool)obj; }
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

    bool valid(const T* obj, s_state) const { return dynamic_cast<Y*>(obj); }
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

    bool valid(const boost::shared_ptr<T>& obj, s_state) const { return (bool)boost::shared_dynamic_cast<Y>(obj); }
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

    bool valid(const boost::intrusive_ptr<T>& obj, s_state) const { return dynamic_cast<Y*>(obj.get()); }
};

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

/** class serializer/deserializes object to the element */
template< typename T,
          typename Policy = default_serialization_policy<T>,
          typename Constructor = default_constructor<T> >
class default_loader
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;
	typedef Constructor			               constructor_type;

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
        if ( policy.valid(obj, LOAD) ) {
            policy.load(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid() const { return policy.valid(obj, LOAD); }

private:
    T&          obj;
    Constructor cons;
    Policy      policy;
};

/** class serializer/deserializes object to the element */
template<typename T, typename Policy>
class default_loader< T, Policy, disable_contructor<T> >
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;
	typedef disable_contructor<T>			   constructor_type;

public:
    explicit default_loader( T&					obj_, 
                             constructor_type	cons_ = constructor_type(),
                             Policy				policy_ = Policy() ) :
        obj(obj_),
        policy(policy_)
    {}

    /** deserialize item from the element */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) 
    { 
        if ( policy.valid(obj, LOAD) ) {
            policy.load(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid() const { return policy.valid(obj, LOAD); }

private:
    T&          obj;
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

    /** serialize item into the element */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    { 
        if ( policy.valid(obj, SAVE) ) {
            policy.save(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid() const { return policy.valid(obj, SAVE); }

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
    typedef typename Policy::xmlpp_holder_type  xmlpp_holder_type;
    typedef Constructor                         constructor_type;

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
        if ( policy.valid(obj, LOAD) ) {
            policy.load(d, e, obj);
        }
    }

    /** serialize item into the element */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    { 
        if ( policy.valid(obj, SAVE) ) {
            policy.save(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid(s_state s) const { return policy.valid(obj, s); }

private:
    T&          obj;
    Constructor cons;
    Policy      policy;
};

/** class serializer/deserializes object to the element */
template<typename T, typename Policy>
class default_serializer< T, Policy, disable_contructor<T> >
{
public:
    typedef typename Policy::xmlpp_holder_type xmlpp_holder_type;
    typedef disable_contructor<T>              constructor_type;

public:
    explicit default_serializer( T&					   obj_, 
                                 disable_contructor<T> cons_ = disable_contructor<T>(),
                                 Policy				   policy_ = Policy() ) :
        obj(obj_),
        policy(policy_)
    {}

    /** deserialize item from the element */
    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) 
    { 
        if ( policy.valid(obj, LOAD) ) {
            policy.load(d, e, obj);
        }
    }

    /** serialize item into the element */
    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const 
    { 
        if ( policy.valid(obj, SAVE) ) {
            policy.save(d, e, obj);
        }
    }

    /** Check for validness */
    bool valid(s_state s) const { return policy.valid(obj, s); }

private:
    T&          obj;
    Policy      policy;
};

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_HELPERS_HPP
