#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "tinyxml.h"
#include <sstream>

namespace xmlpp
{
    /**
     * Class for working with xml attributes
     */
    class Attribute
    {
    private:
        friend class Element;

    private:
        TiXmlAttribute* pAttribute;

    public:
        // Construct
        Attribute(const Attribute& rhs) : pAttribute(rhs.pAttribute) {}
        explicit Attribute(TiXmlAttribute* pAttr) : pAttribute(pAttr) {}

        /**
         * Get name of the attribute
         * @return attribute name
         */
        std::string Name() const;

        /**
         * Get attrbiute value
         * @return value of the attribute
         */
        std::string Value() const;

        /**
         * Get attrbiute depending on the type. Assert if fails
         * @param output value
         * @return value of the attribute
         */
        template<class value_t>
        value_t& Value(value_t& outValue) const
        {
            std::stringstream sstream( Value() );
            sstream >> outValue;
            return outValue;
        }
    };
}

#endif // ATTRIBUTE_H
