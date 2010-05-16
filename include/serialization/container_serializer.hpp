#ifndef XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
#define XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP

#include "generic_serializer.hpp"
#include "text_serializer.hpp"

namespace xmlpp {

template<typename Container, typename Serializer>
class container_serializer
{
public:
    typedef element xmlpp_holder_type;

public:
    container_serializer(const std::string& name_, Container& items_) :
        name(name_),
        items(items_) 
    {}

    template<typename Document>
    void load(const Document& d, const xmlpp_holder_type& e) const
    {
        typename Container::value_type value;
        Serializer serializer(value);

        for (const_element_iterator i  = e.first_child_element();
                                    i != e.end_child_element();
                                    ++i)
        {
            if (i->get_value() == name) 
            {
                serializer.load(d, *i);
                items.push_back(value);
            }
        }
    }

    template<typename Document>
    void save(Document& d, xmlpp_holder_type& e) const
    {
        for (typename Container::iterator i  = items.begin();
                                          i != items.end();
                                          ++i)
        {
            xmlpp::element elem(name);
            Serializer(*i).save(d, elem);
            add_child(e, elem);
        }
    }

private:
    std::string name;
    Container&  items;
};

template<typename Container>
container_serializer<Container, typename Container::reference> as_element_set(const std::string& name, Container& items)
{
    return container_serializer<Container, typename Container::reference>(name, items);
}

template<typename Container>
container_serializer< Container, text_serializer<typename Container::value_type> > as_text_element_set(const std::string& name, Container& items)
{
    return container_serializer< Container, text_serializer<typename Container::value_type> >(name, items);
}

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_CONTAINER_SERIALIZER_HPP
