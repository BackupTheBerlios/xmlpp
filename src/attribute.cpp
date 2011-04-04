#include "attribute.h"

using namespace xmlpp;

const char* attribute::get_name() const
{
    assert(tixmlAttribute);
    return tixmlAttribute->Name();
}

const char* attribute::get_value() const
{
    assert(tixmlAttribute);
    return tixmlAttribute->Value();
}

void attribute::set_name(const char* name) const
{
    tixmlAttribute->SetName(name);
}

void attribute::set_value(const char* value) const
{
    tixmlAttribute->SetValue(value);
}