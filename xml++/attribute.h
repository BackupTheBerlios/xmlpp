#ifndef XMLPP_ATTRIBUTE_H
#define XMLPP_ATTRIBUTE_H

#include "tinyxml.h"
#include <sstream>

namespace xmlpp {

/**
 * Class for working with xml attributes
 */
class attribute
{
private:
    friend class element;

private:
    TiXmlAttribute* tixmlAttribute;

public:
    // Construct
    attribute(const attribute& rhs) : 
        tixmlAttribute(rhs.tixmlAttribute) {}
    explicit attribute(TiXmlAttribute* _tixmlAttribute) : 
        tixmlAttribute(_tixmlAttribute) {}

    /** Get name of the attribute
     * @return attribute name
     */
    const char* get_name() const;

    /** Get attrbiute value
     * @return value of the attribute
     */
    const char* get_value() const;

    /** Set attribute name */
    void set_name(const char* name) const;

    /** Set attribute value */
    void set_value(const char* value) const;

    /**
     * Get attrbiute depending on the type. Assert if fails
     * @param output value
     * @return value of the attribute
     */
    template<typename value_t>
    value_t& get_value(value_t& outValue) const
    {
        std::stringstream sstream( get_value() );
        sstream >> outValue;
        return outValue;
    }

    /** Compare nodes */
    bool operator == (const attribute& rhs) const { return tixmlAttribute == rhs.tixmlAttribute; }
};

} // namepsace xmlpp

#endif // XMLPP_ATTRIBUTE_H
