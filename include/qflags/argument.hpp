// argument.hpp
//

#include <qflags/qflags.h>

namespace qflags {

////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QFLAGS_INLINE argument::argument(char const* name,
                                 char const* short_name,
                                 char const* description) :
    _name(name),
    _short_name(short_name),
    _description(description),
    _is_set(false) {}

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

//------------------------------------------------------------------------------
QFLAGS_INLINE argument const& argument::value_array(size_t const) const
{
    throw std::logic_error(_name + " is not an array.");
}

} // namespace qflags
