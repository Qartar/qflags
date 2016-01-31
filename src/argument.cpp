// argument.cpp
//
#pragma once

#include <qflags/qflags.h>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
argument::argument(char const* name) :
    _name(name),
    _is_set(false) {}

//------------------------------------------------------------------------------
bool argument::is_flag() const
{
    return false;
}

//------------------------------------------------------------------------------
bool argument::is_boolean() const
{
    return false;
}

//------------------------------------------------------------------------------
bool argument::is_integer() const
{
    return false;
}

//------------------------------------------------------------------------------
bool argument::is_string() const
{
    return false;
}

//------------------------------------------------------------------------------
bool argument::value_boolean() const
{
    throw std::logic_error(_name + " is not convertible to a boolean.");
}

//------------------------------------------------------------------------------
int64_t argument::value_integer() const
{
    throw std::logic_error(_name + " is not convertible to an integer.");
}

//------------------------------------------------------------------------------
std::string const& argument::value_string() const
{
    throw std::logic_error(_name + " is not convertible to a string.");
}

} // namespace qflags
