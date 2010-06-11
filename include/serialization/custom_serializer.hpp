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
        function(d, n, SERIALIZATION_LOAD);
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        function(d, n, SERIALIZATION_SAVE);
    }

    template<typename Document>
    void serialize(Document& d, xmlpp_holder_type& n, xmlpp::serialization_state state)
    {
        function(d, n, state);
    }

private:
    Function function;
};

template<typename Function>
custom_loader<Function> as_custom_loader(Function function)
{
    return custom_loader<Function>(function);
}

template<typename Function>
custom_saver<Function> as_custom_saver(Function function)
{
    return custom_saver<Function>(function);
}

template<typename Function>
custom_serializer<Function> as_custom_serializer(Function function)
{
    return custom_serializer<Function>(function);
}

template<typename Function>
custom_loader<Function, attribute> as_attr_loader(Function function)
{
    return custom_loader<Function, attribute>(function);
}

template<typename Function>
custom_saver<Function, attribute> as_attr_saver(Function function)
{
    return custom_saver<Function, attribute>(function);
}

template<typename Function>
custom_serializer<Function, attribute> as_attr_serializer(Function function)
{
    return custom_serializer<Function, attribute>(function);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CUSTOM_SERIALIZER_HPP
