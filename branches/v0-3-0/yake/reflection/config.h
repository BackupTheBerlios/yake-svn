#ifndef _YAKE_REFLECTION_CONFIG_H_
#define _YAKE_REFLECTION_CONFIG_H_


namespace yake
{
namespace reflection
{

/* compile time flags */
// dll import/export
#if defined( YAKE_REFLECTION_EXPORTS )
#	define YAKE_REFLECTION_API DLLEXPORT
#else
#	define YAKE_REFLECTION_API DLLIMPORT
#endif

// luabind versions
#define YAKE_REFLECTION_LUABIND_DISABLED -1
#define YAKE_REFLECTION_LUABIND_6 60
#define YAKE_REFLECTION_LUABIND_7RC3 73
#define YAKE_REFLECTION_LUABIND_7RC4 74

#define YAKE_REFLECTION_LUABIND_VER YAKE_REFLECTION_LUABIND_7RC4

#if(YAKE_REFLECTION_LUABIND_VER == YAKE_REFLECTION_NO_LUABIND)
#	pragma message("Yake reflection config: yake.reflection.config.luabind_disabled")
#elif(YAKE_REFLECTION_LUABIND_VER == YAKE_REFLECTION_LUABIND_6)
#	pragma message("Yake reflection config: yake.reflection.config.luabind_6")
#elif(YAKE_REFLECTION_LUABIND_VER == YAKE_REFLECTION_LUABIND_7RC3)
#	pragma message("Yake reflection config: yake.reflection.config.luabind_7rc3")
#elif(YAKE_REFLECTION_LUABIND_VER == YAKE_REFLECTION_LUABIND_7RC4)
#	pragma message("Yake reflection config: yake.reflection.config.luabind_7rc4")
#endif

} // namespace reflection
} // namespace yake

#endif // _YAKE_REFLECTION_CONFIG_H_