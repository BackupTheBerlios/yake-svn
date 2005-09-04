#ifndef _YAKE_REFLECTION_LUABIND_CLASS_REGISTRATION_H_
#define _YAKE_REFLECTION_LUABIND_CLASS_REGISTRATION_H_

#include <luabind/lua_include.hpp>

#include <luabind/config.hpp>
#include <luabind/class.hpp>

#include <cstring>
#include <iostream>

namespace luabind { namespace detail {

/* class_registration declaration needed because in luabind this sits within a compilation unit and
	 is not directly accessable, so we simply hold our own declaration */

    struct class_registration : registration
    {   
        class_registration(char const* name);

        void register_(lua_State* L) const;

        const char* m_name;

        mutable std::list<detail::method_rep> m_methods;

        // datamembers, some members may be readonly, and
        // only have a getter function
        mutable std::map<const char*, detail::class_rep::callback, detail::ltstr> m_getters;
        mutable std::map<const char*, detail::class_rep::callback, detail::ltstr> m_setters;

        // the operators in lua
        mutable std::vector<detail::class_rep::operator_callback> m_operators[detail::number_of_operators]; 
        mutable std::map<const char*, int, detail::ltstr> m_static_constants;

        mutable std::vector<class_base::base_desc> m_bases;
        mutable detail::construct_rep m_constructor;

        void(*m_destructor)(void*);
        void(*m_const_holder_destructor)(void*);

        void*(*m_extractor)(void*);
        const void*(*m_const_extractor)(void*);

        void(*m_const_converter)(void*,void*);

        void(*m_construct_holder)(void*, void*);
        void(*m_construct_const_holder)(void*, void*);

        void(*m_default_construct_holder)(void*);
        void(*m_default_construct_const_holder)(void*);

		void(*m_adopt_fun)(void*);

        int m_holder_size;
        int m_holder_alignment;

        LUABIND_TYPE_INFO m_type;
        LUABIND_TYPE_INFO m_holder_type;
        LUABIND_TYPE_INFO m_const_holder_type;

        scope m_scope;
    };

}} // namespace luabind::detail

#endif // _YAKE_REFLECTION_LUABIND_CLASS_REGISTRATION_H_