// qflags_gtest.h
//

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:6011) // Dereferencing NULL pointer.
#pragma warning(disable:6330) // 'const char' passed as _Param_(1) when 'unsigned char' is required in call to 'isalnum'.
#pragma warning(disable:28182) // Dereferencing NULL pointer.
#endif //defined(_MSC_VER)

#include <gtest/gtest.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif //defined(_MSC_VER)
