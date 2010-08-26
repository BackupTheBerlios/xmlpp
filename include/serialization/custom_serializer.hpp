#ifndef XMLPP_SERIALIZATION_CUSTOM_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_CUSTOM_SERIALIZER_HPP

#include "../attribute.h"
#include "../node.h"

namespace xmlpp {

template<typename Function, typename Holder = element>
class custom_loader
{
public:
    typedef Holder xmlpp_holder_type;

public:
    custom_loader(Function function_) :
        function(function_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        function(d, e);
    }

    template<typename Document>
    void save(Document&, xmlpp_holder_type&) const
    {
        assert(!"Not implemented");
    }

private:
    Function function;
};

template<typename Function, typename Holder = element>
class custom_saver
{
public:
    typedef Holder xmlpp_holder_type;

public:
    custom_saver(Function function_) :
        function(function_)
    {}

    template<typename Document>
    void load(const Document&, const xmlpp_holder_type&)
    {
        assert(!"Not implemented");
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        function(d, e);
    }

private:
    Function function;
};

template<typename Function, typename Holder = element>
class custom_serializer
{
public:
    typedef Holder xmlpp_holder_type;

public:
    custom_serializer(Function function_) :
        function(function_)
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e)
    {
        function(d, e, LOAD);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        function(d, e, SAVE);
    }

    template<typename Document>
    void serialize(Document& d, xmlpp_holder_type& n, xmlpp::s_state state)
    {
        function(d, n, state);
    }

private:
    Function function;
};

/** Setup custom function as element loader */
template<typename Function>
custom_loader<Function, element> make_elem_loader(Function function)
{
    return custom_loader<Function>(function);
}

/** Setup custom function as element saver */
template<typename Function>
custom_saver<Function, element> make_elem_saver(Function function)
{
    return custom_saver<Function>(function);
}

/** Setup custom function as element serializer */
template<typename Function>
custom_serializer<Function, element> make_elem_serializer(Function function)
{
    return custom_serializer<Function>(function);
}

/** Setup custom function as attribute loader */
template<typename Function>
custom_loader<Function, attribute> make_attr_loader(Function function)
{
    return custom_loader<Function, attribute>(function);
}

/** Setup custom function as attribute saver */
template<typename Function>
custom_saver<Function, attribute> make_attr_saver(Function function)
{
    return custom_saver<Function, attribute>(function);
}

/** Setup custom function as attribute serializer */
template<typename Function>
custom_serializer<Function, attribute> make_attr_serializer(Function function)
{
    return custom_serializer<Function, attribute>(function);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CUSTOM_SERIALIZER_HPP
