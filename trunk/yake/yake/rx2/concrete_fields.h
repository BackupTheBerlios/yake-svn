#ifndef concrete_fields_h
#define concrete_fields_h

#include "typed_field.h"
#include <string>

namespace rx
{

// we need to define concrete fields, because luabind
// cannot handle template functions, the network backend
// needs concrete type information and the serialization
// library needs to register the type infos
typedef typed_field<bool> bool_field;
typedef typed_field<int> int_field;
typedef typed_field<float> float_field;
typedef typed_field<std::string> string_field;

} // namespace rx

#endif