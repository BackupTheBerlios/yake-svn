#ifndef concrete_fields_h
#define concrete_fields_h

#include "typed_field.h"
#include <string>

namespace rx
{

typedef typed_field<bool> bool_field;
typedef typed_field<int> int_field;
typedef typed_field<float> float_field;
typedef typed_field<std::string> string_field;

} // namespace rx

#endif