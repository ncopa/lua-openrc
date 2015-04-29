lua-openrc
==========

Lua binding to OpenRC C library

Functions
---------

* `runlevel_get()` - Return the current runlevel. 
* `runlevel_exists(runlevel)` - Checks if the runlevel exists or not 
* `runlevel_list()` - Return a list of runlevels. 
* `service_add(service, [runlevel])` - Add the service to the runlevel 
* `service_delete(service, [runlevel])` - Remove the service from the runlevel 
* `service_description(service, [option])` - Returns a description of what the service and/or option does. 
* `service_exists(service)` - Checks if a service exists or not.
* `service_in_runlevel(service, [runlevel])` - Checks if a service is in a runlevel
* `service_resolve(service)` - Resolves a service name to its full path. 
* `service_extra_commands(service)` - Lists the extra commands a service has. 
* `service_states()` - List all available states 
* `service_status(service, [state])` - Return current state the service is in or test service against given state
* `services_in_runlevel([runlevel])` - List the services in a runlevel or all services 
* `services_in_runlevel_stacked([runlevel])` - List the stacked services in a runlevel 
* `services_in_state(rcstate)` - List the services in a state 
* `services_scheduled(service)` - List the services shceduled to start when this one does 
* `service_daemons_crashed(service)` - Checks that all daemons started with start-stop-daemon by the service are still running. 
* `sys()` - name system type (i.e VSERVER, OPENVZ, UML...) 
