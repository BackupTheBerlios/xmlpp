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

void attribute::set_name(const std::string& name) const
{
    tixmlAttribute->SetName(name);
}

void attribute::set_value(const std::string& value) const
{
    tixmlAttribute->SetValue(value);
}