#ifndef XMLPP_SERIALIZATION_NAME_VALUE_PAIR_HPP
#define XMLPP_SERIALIZATION_NAME_VALUE_PAIR_HPP

#include "../node.h"

namespace xmlpp  {

#define XMLPP_STORE_AS(type)    typedef type    xmlpp_holder_type

// forward
template<typename Serializer>
class name_value_pair;

// little generic
struct nvp_list_end {};

template<typename S, typename Rest>
struct nvp_list
{
    typedef S                                               serializer_type;
    typedef typename serializer_type::xmlpp_holder_type     xmlpp_holder_type;

    name_value_pair<S>  nvp;
    Rest                rest;

    nvp_list(const name_value_pair<S>& nvp_,
             const Rest&               rest_) :
        nvp(nvp_),
        rest(rest_)
    {
    }
};

template<typename Serializer>
class name_value_pair
{
public:
    typedef Serializer                                  serializer_type;
    typedef typename serializer_type::xmlpp_holder_type xmlpp_holder_type;

public:
    name_value_pair(const std::string&  name_, 
                    serializer_type     serializer_) :
        name(name_),
        serializer(serializer_)
    {}

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& h) const   
    {
        unroll_nvp_and_save(serializer, d, h);
    }

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& h) 
    { 
        unroll_nvp_and_load(serializer, d, h);
    }

public:
    const std::string   name;
    serializer_type     serializer;
};

template<typename Serializer>
name_value_pair<Serializer> make_nvp(const std::string& name, const Serializer& serializer)
{
    return name_value_pair<Serializer>(name, serializer);
}

template<typename S0, typename S1, typename Rest>
nvp_list< S0, nvp_list<S1, Rest> > operator & (const nvp_list<S1, Rest>&    nvpl,
                                               const name_value_pair<S0>&   nvp)
{
    return nvp_list< S0, nvp_list<S1, Rest> >(nvp, nvpl);
}

template<typename S0, typename S1>
nvp_list< S0, nvp_list<S1, nvp_list_end> > operator & (const name_value_pair<S0>& nvp0,
                                                       const name_value_pair<S1>& nvp1)
{
    return nvp_list<S1, nvp_list_end>( nvp1, nvp_list_end() ) & nvp0;
}

// unroll nvp list as generic serializer and load
template<typename Document, typename S, typename Rest, typename Holder>
void unroll_nvp_and_load(nvp_list<S, Rest>& nvpl,
                         const Document&    d,
                         const Holder&  n)
{
    generic_serializer<Document> gs;
    gs &= nvpl;
    gs.load(d, n);
}

// unroll nvp as generic serializer and load
template<typename Document, typename Serializer, typename Holder>
void unroll_nvp_and_load(name_value_pair<Serializer>&   nvp,
                         const Document&                d,
                         const Holder&                  n)
{
    generic_serializer<Document> gs;
    gs &= nvp;
    gs.load(d, n);
}

// No nvp in serializers
template<typename Document, typename Serializer, typename Holder>
void unroll_nvp_and_load(Serializer&        s,
                         const Document&    d,
                         const Holder&      n)
{
    s.load(d, n);
}

// unroll nvp list as generic serializer and save
template<typename Document, typename S, typename Rest, typename Holder>
void unroll_nvp_and_save(const nvp_list<S, Rest>&   nvpl,
                         Document&                  d,
                         Holder&                    n)
{
    generic_serializer<Document> gs;
    gs &= nvpl;
    gs.save(d, n);
}

// unroll nvp as generic serializer and save
template<typename Document, typename Serializer, typename Holder>
void unroll_nvp_and_save(const name_value_pair<Serializer>& nvp,
                         Document&                          d,
                         Holder&                            n)
{
    generic_serializer<Document> gs;
    gs &= nvp;
    gs.save(d, n);
}

// No nvp in serializers
template<typename Document, typename Serializer, typename Holder>
void unroll_nvp_and_save(const Serializer&  s,
                         Document&          d,
                         Holder&            n)
{
    s.save(d, n);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_NAME_VALUE_PAIR_HPP
