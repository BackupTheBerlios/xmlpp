#include "Attribute.h"

using namespace xmlpp;

/**
 * Get name of the attribute
 * @return attribute name
 */
std::string Attribute::Name() const
{
    assert(pAttribute);
    return pAttribute->Name();
}

/**
 * Get attrbiute value
 * @return value of the attribute
 */
std::string Attribute::Value() const
{
    assert(pAttribute);
    return pAttribute->ValueStr();
}
