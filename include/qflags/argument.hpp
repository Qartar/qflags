// argument.hpp
//

#include <qflags/qflags.h>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QFLAGS_INLINE argument::argument(char const* name) :
    _name(name),
    _is_set(false) {}

//------------------------------------------------------------------------------
QFLAGS_INLINE bool argument::is_flag() const
{
    return false;
}

//------------------------------------------------------------------------------
QFLAGS_INLINE bool argument::is_boolean() const
{
    return false;
}

//------------------------------------------------------------------------------
QFLAGS_INLINE bool argument::is_integer() const
{
    return false;
}

//------------------------------------------------------------------------------
QFLAGS_INLINE bool argument::is_string() const
{
    return false;
}

//------------------------------------------------------------------------------
QFLAGS_INLINE bool argument::value_boolean() const
{
    throw std::logic_error(_name + " is not convertible to a boolean.");
}

//------------------------------------------------------------------------------
QFLAGS_INLINE int64_t argument::value_integer() const
{
    throw std::logic_error(_name + " is not convertible to an integer.");
}

//------------------------------------------------------------------------------
QFLAGS_INLINE std::string const& argument::value_string() const
{
    throw std::logic_error(_name + " is not convertible to a string.");
}

} // namespace qflags
