
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <rc.h>

#define MYNAME		"rc"
#define MYVERSION	"OpenRC library for " LUA_VERSION " / Jan 2010"

/** runlevel_get() - Return the current runlevel. */
static int Prunlevel_get(lua_State *L)
{
	lua_pushstring(L, rc_runlevel_get());
	return 1;
}

/** runlevel_exists(runlevel) - Checks if the runlevel exists or not */
static int Prunlevel_exists(lua_State *L)
{
	lua_pushboolean(L, rc_runlevel_exists(luaL_checkstring(L, 1)));
	return 1;
}

/** runlevel_list() - Return a list of runlevels. */
static int Prunlevel_list(lua_State *L)
{
	int i = 1;
	RC_STRING *item;
	RC_STRINGLIST *list = rc_runlevel_list();
	lua_newtable(L);
	TAILQ_FOREACH(item, list, entries) {
		lua_pushnumber(L, i++);
		lua_pushstring(L, item->value);
		lua_settable(L, -3);
	}
	rc_stringlist_free(list);
	return 1;
}


/** service_add(service, [runlevel]) - Add the service to the runlevel */
static int Pservice_add(lua_State *L)
{
	const char *service, *runlevel;
	service =luaL_checkstring(L, 1);
	runlevel = luaL_optstring(L, 2, rc_runlevel_get());
	lua_pushboolean(L, rc_service_add(runlevel, service));
	return 1;
}

/** service_delete(service, [runlevel]) - Remove the service from the runlevel */
static int Pservice_delete(lua_State *L)
{
	const char *service, *runlevel;
	service =luaL_checkstring(L, 1);
	runlevel = luaL_optstring(L, 2, rc_runlevel_get());
	lua_pushboolean(L, rc_service_delete(runlevel, service));
	return 1;
}

/** service_description(service, [option]) - Returns a description of what the service and/or option does. */
static int Pservice_description(lua_State *L)
{
	const char *service = luaL_checkstring(L, 1);
	const char *option = luaL_optstring(L, 2, NULL);
	lua_pushstring(L, rc_service_description(service, option));
	return 1;
}

/** service_exists(service) - Checks if a service exists or not.*/
static int Pservice_exists(lua_State *L)
{
	const char *service = luaL_checkstring(L, 1);
	lua_pushboolean(L, rc_service_exists(service));
	return 1;
}

/** service_in_runlevel(service, [runlevel]) - Checks if a service is in a runlevel*/
static int Pservice_in_runlevel(lua_State *L)
{
	const char *service, *runlevel;
	service =luaL_checkstring(L, 1);
	runlevel = luaL_optstring(L, 2, rc_runlevel_get());
	lua_pushboolean(L, rc_service_in_runlevel(service, runlevel));
	return 1;
}

/** service_resolve(service) - Resolves a service name to its full path. */
static int Pservice_resolve(lua_State *L)
{
	const char *service;
	service =luaL_checkstring(L, 1);
	lua_pushstring(L, rc_service_resolve(service));
	return 1;
}

/** service_extra_commands(service) - Lists the extra commands a service has. */
static int Pservice_extra_commands(lua_State *L)
{
	int i = 1;
	RC_STRING *item;
	const char *service = luaL_checkstring(L, 1);
	RC_STRINGLIST *list = rc_service_extra_commands(service);
	lua_newtable(L);
	TAILQ_FOREACH(item, list, entries) {
		if (item->value == NULL || item->value[0] == '\0')
			continue;
		lua_pushnumber(L, i++);
		lua_pushstring(L, item->value);
		lua_settable(L, -3);
	}
	rc_stringlist_free(list);
	return 1;
}


static const luaL_reg R[] =
{
	{"runlevel_get", 		Prunlevel_get},
	{"runlevel_exists",		Prunlevel_exists},
	{"runlevel_list",		Prunlevel_list},

	{"service_add",			Pservice_add},
	{"service_delete",		Pservice_delete},
	{"service_description",	Pservice_description},
	{"service_exists",		Pservice_exists},
	{"service_in_runlevel",		Pservice_in_runlevel},
	{"service_resolve",		Pservice_resolve},
	{"service_extra_commands", Pservice_extra_commands},
	{NULL,				NULL}
};

#define set_const(key, value)		\
	lua_pushliteral(L, key);	\
	lua_pushnumber(L, value);	\
	lua_settable(L, -3)

LUALIB_API int luaopen_rc (lua_State *L)
{
	luaL_register(L, MYNAME, R);

	lua_pushliteral(L,"version");		/** version */
	lua_pushliteral(L,MYVERSION);
	lua_settable(L,-3);
	
	return 1;
}

