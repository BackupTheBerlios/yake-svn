#include "bind_lua/luabind_class_registration.h"

namespace luabind { namespace detail {

    class_registration::class_registration(char const* name)
    {
        m_name = name;
    }

    void class_registration::register_(lua_State* L) const
    {
        LUABIND_CHECK_STACK(L);

        assert(lua_type(L, -1) == LUA_TTABLE);

        lua_pushstring(L, m_name);

        detail::class_rep* crep;

        detail::class_registry* r = detail::class_registry::get_registry(L);
        // create a class_rep structure for this class.
        // allocate it within lua to let lua collect it on
        // lua_close(). This is better than allocating it
        // as a static, since it will then be destructed
        // when the program exits instead.
        // warning: we assume that lua will not
        // move the userdata memory.
        lua_newuserdata(L, sizeof(detail::class_rep));
        crep = reinterpret_cast<detail::class_rep*>(lua_touserdata(L, -1));

        new(crep) detail::class_rep(
            m_type
            , m_name
            , L
            , m_destructor
            , m_const_holder_destructor
            , m_holder_type
            , m_const_holder_type
            , m_extractor
            , m_const_extractor
            , m_const_converter
            , m_construct_holder
            , m_construct_const_holder
            , m_default_construct_holder
            , m_default_construct_const_holder
			, m_adopt_fun
            , m_holder_size
            , m_holder_alignment);

        // register this new type in the class registry
        r->add_class(m_type, crep);
        if (!(LUABIND_TYPE_INFO_EQUAL(m_holder_type, LUABIND_INVALID_TYPE_INFO)))
        {
            // if we have a held type
            // we have to register it in the class-table
            // but only for the base class, if it already
            // exists, we don't have to register it
            detail::class_rep* c = r->find_class(m_holder_type);
            if (c == 0)
            {
                r->add_class(m_holder_type, crep);
                r->add_class(m_const_holder_type, crep);
            }
        }

        // constructors
        m_constructor.swap(crep->m_constructor);

        crep->m_getters.swap(m_getters);
        crep->m_setters.swap(m_setters);

        for (int i = 0; i < detail::number_of_operators; ++i)
            crep->m_operators[i].swap(m_operators[i]);

        crep->m_static_constants.swap(m_static_constants);

		typedef std::list<detail::method_rep> methods_t;

		detail::class_registry* registry = detail::class_registry::get_registry(L);

        for (std::vector<class_base::base_desc>::iterator i = m_bases.begin();
            i != m_bases.end(); ++i)
        {
            LUABIND_CHECK_STACK(L);

            // the baseclass' class_rep structure
            detail::class_rep* bcrep = registry->find_class(i->type);

            detail::class_rep::base_info base;
            base.pointer_offset = i->ptr_offset;
            base.base = bcrep;

            crep->add_base_class(base);

            // copy base class table
			crep->get_table(L);
			bcrep->get_table(L);
            lua_pushnil(L);

            while (lua_next(L, -2))
            {
                lua_pushvalue(L, -2); // copy key
                lua_insert(L, -2);
                lua_settable(L, -5);
            }
            lua_pop(L, 2);

            // copy base class detaults table
			crep->get_default_table(L);
			bcrep->get_default_table(L);
            lua_pushnil(L);

            while (lua_next(L, -2))
            {
                lua_pushvalue(L, -2); // copy key
                lua_insert(L, -2);
                lua_settable(L, -5);
            }
            lua_pop(L, 2);

		}

        // add methods
        for (std::list<detail::method_rep>::iterator i 
            = m_methods.begin(); i != m_methods.end(); ++i)
        {
            LUABIND_CHECK_STACK(L);
			crep->add_method(*i);
		}

		crep->register_methods(L);

        m_methods.clear();

        crep->get_default_table(L);
        m_scope.register_(L);
        lua_pop(L, 1);

        lua_settable(L, -3);
    }

}} // namespace luabind::detail