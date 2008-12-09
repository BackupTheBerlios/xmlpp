#include "attribute.h"

using namespace xmlpp;

std::string attribute::get_name() const
{
    assert(tixmlAttribute);
    return tixmlAttribute->Name();
}

std::string attribute::get_value() const
{
    assert(tixmlAttribute);
    return tixmlAttribute->ValueStr();
}
