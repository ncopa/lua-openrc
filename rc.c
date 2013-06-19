
#include <string.h>

#include <rc.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#if LUA_VERSION_NUM < 502
# define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

#define MYNAME		"rc"
#define MYVERSION	"OpenRC library for " LUA_VERSION " version " VERSION

/* convert RC_STRINGLIST to a lua table on the stack */
static int push_stringlist(lua_State *L, RC_STRINGLIST *list)
{
	int i = 0;
	RC_STRING *item;
	lua_newtable(L);
	TAILQ_FOREACH(item, list, entries) {
		if (item->value == NULL || item->value[0] == '\0')
			continue;
		lua_pushnumber(L, ++i);
		lua_pushstring(L, item->value);
		lua_settable(L, -3);
	}
	return i;
}

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
	RC_STRINGLIST *list = rc_runlevel_list();
	push_stringlist(L, list);
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
	service = luaL_checkstring(L, 1);
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
	const char *service = luaL_checkstring(L, 1);
	RC_STRINGLIST *list = rc_service_extra_commands(service);
	push_stringlist(L, list);
	rc_stringlist_free(list);
	return 1;
}

struct statestr {
	RC_SERVICE state;
	const char *str;
};

static struct statestr states[] = {
	{ RC_SERVICE_STOPPED, "stopped" },
	{ RC_SERVICE_STARTED, "started" },
	{ RC_SERVICE_STOPPING, "stopping" },
	{ RC_SERVICE_STARTING, "starting" },
	{ RC_SERVICE_INACTIVE, "inactive" },
	{ RC_SERVICE_HOTPLUGGED, "hotplugged" },
	{ RC_SERVICE_FAILED,	"failed" },
	{ RC_SERVICE_SCHEDULED, "scheduled" },
	{ RC_SERVICE_WASINACTIVE, "wasinactive" },
	{ 0, NULL },
};


static RC_SERVICE get_rcstate(const char *str)
{
	int i;
	for (i = 0; states[i].state != 0; i++)
			if (strcmp(states[i].str, str) == 0)
				return states[i].state;
	return 0;
}

/** service_states() - List all available states */
static int Pservice_states(lua_State *L)
{
	int i;
	lua_newtable(L);
	for (i = 0; states[i].state != 0; i++) {
		lua_pushnumber(L, i+1);
		lua_pushstring(L, states[i].str);
		lua_settable(L, -3);
	}
	return 1;	
}

/** service_status(service, [state]) - Return current state the service is in or test service against given state*/
static int Pservice_status(lua_State *L)
{
	int i, n = 0;
	const char *service = luaL_checkstring(L, 1);
	const char *opt = luaL_optstring(L, 2, NULL);
	RC_SERVICE svc_state = rc_service_state(service);
	if (opt != NULL) {
		/* return boolean if service is in give option */
		lua_pushboolean(L, get_rcstate(opt) & svc_state);
		return 1;
	}

	/* return all states that given service is in */	
	lua_newtable(L);
	for (i = 0; states[i].state != 0; i++) {
		if (svc_state & states[i].state) {
			lua_pushstring(L, states[i].str);
			n++;
		}
	}
	return n;
}

/** services_in_runlevel([runlevel]) - List the services in a runlevel or all services */
static int Pservices_in_runlevel(lua_State *L)
{
	const char *runlevel = luaL_optstring(L, 1, NULL);
	RC_STRINGLIST *list = rc_services_in_runlevel(runlevel);
	push_stringlist(L, list);
	rc_stringlist_free(list);
	return 1;
}

/** services_in_runlevel_stacked([runlevel]) - List the stacked services in a runlevel */
static int Pservices_in_runlevel_stacked(lua_State *L)
{
	const char *runlevel = luaL_optstring(L, 1, NULL);
	RC_STRINGLIST *list = rc_services_in_runlevel_stacked(runlevel);
	push_stringlist(L, list);
	rc_stringlist_free(list);
	return 1;
}


/** services_in_state(rcstate) - List the services in a state */
static int Pservices_in_state(lua_State *L)
{
	const char *str = luaL_checkstring(L, 1);
	RC_STRINGLIST *list = rc_services_in_state(get_rcstate(str));
	push_stringlist(L, list);
	rc_stringlist_free(list);
	return 1;
}

/** services_scheduled(service) - List the services shceduled to start when this one does */
static int Pservices_scheduled(lua_State *L)
{
	const char *service = luaL_checkstring(L, 1);
	RC_STRINGLIST *list = rc_services_scheduled(service);
	push_stringlist(L, list);
	rc_stringlist_free(list);
	return 1;
}

/** service_daemons_crashed(service) - Checks that all daemons started with start-stop-daemon by the service are still running. */
static int Pservice_daemons_crashed(lua_State *L)
{
	const char *service = luaL_checkstring(L, 1);
	lua_pushboolean(L, rc_service_daemons_crashed(service));
	return 1;
}

/** sys() - name system type (i.e VSERVER, OPENVZ, UML...) */
static int Psys(lua_State *L)
{
	lua_pushstring(L, rc_sys());
	return 1;
}

static const luaL_Reg R[] =
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
	{"service_states",		Pservice_states},
	{"service_status",		Pservice_status},
	{"services_in_runlevel",Pservices_in_runlevel},
	{"services_in_runlevel_stacked",Pservices_in_runlevel_stacked},
	{"services_in_state",	Pservices_in_state},
	{"services_scheduled",	Pservices_scheduled},
	{"service_daemons_crashed", Pservice_daemons_crashed},
	{"sys",					Psys},
	{NULL,				NULL}
};

#define set_literal(key, value)		\
	lua_pushliteral(L, key);	\
	lua_pushliteral(L, value);	\
	lua_settable(L, -3)

LUALIB_API int luaopen_rc (lua_State *L)
{
	luaL_newlib(L, R);

	set_literal("version", MYVERSION);	/** version */
	set_literal("initdir", RC_INITDIR);	/** initdir */
	set_literal("confdir", RC_CONFDIR); /** confdir */
	return 1;
}

