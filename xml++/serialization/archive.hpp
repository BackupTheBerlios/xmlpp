#ifndef XMLPP_SERIALIZATION_ARCHIVE_HPP
#define XMLPP_SERIALIZATION_ARCHIVE_HPP

#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <stack>

namespace xmlpp {

/** Input archive for boost::serialization which reads data from xml++ nodes */
class iarchive :
    public boost::archive::detail::common_iarchive<archive>
{
public:    
    // permit serialization system privileged access to permit
    // implementation of inline templates for maximum speed.
    friend class boost::archive::load_access;

};

/** Output archive for boost::serialization which stores data in xml++ nodes */
class oarchive :
    public boost::archive::detail::common_oarchive<archive>
{
public:    
    // permit serialization system privileged access to permit
    // implementation of inline templates for maximum speed.
    friend class boost::archive::save_access;

public:
    void save_start(const char* name)
    {
        element child(name);
        elements.top().add_child(child);
        elements.push(child);
    }

    void save_end(const char* name)
    {
        elements.pop();
    }

    // member template for saving primitive types.
    // Specialize for any types/templates that special treatment
    template<class T>
    void save(T& t)
    {
    }

    template<class T>  
    void save_override(boost::serialization::nvp<T>& t, int)
    {
        save_start( t.name() );
        detail_common_oarchive::save_override(t.value(), 0);
        save_end( t.name() );
    }
private:
    std::stack<element> elements;
};

} // namespace xmlpp

#endif // XMLPP_SERIALIZATION_ARCHIVE_HPP