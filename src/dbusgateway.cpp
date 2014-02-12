/*
 *   Copyright (C) 2014 Pelagicore AB
 *   All rights reserved.
 */
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include "dbusgateway.h"
#include "debug.h"

DBusGateway::DBusGateway(ProxyType type, const std::string &containerRoot,
	const std::string &name):
	m_type(type)
{
	/* TODO: The config should not be set here, it should be set
	 * when Pelagicontain calls setConfig() on this object. This
	 * means dbus-proxy needs to be modified to not take a config
	 * file as argument which it bases it's config on, rather it
	 * should somehow get the config set as a result of the call
	 * to setConfig()
	 */
	std::string configPath = containerRoot + "/config/pelagicontain.conf";

	if (m_type == SessionProxy) {
		m_socket = containerRoot
			+ std::string("/sess_")
			+ name
			+ std::string(".sock");
	} else {
		m_socket = containerRoot
			+ std::string("/sys_")
			+ name
			+ std::string(".sock");
	}

	log_debug("Spawning %s proxy, socket: %s, config: %s",
		typeString(), m_socket.c_str(), configPath.c_str());

	m_pid = fork();
	if (m_pid == 0) { /* child */
		/* This call never returns... */
		execlp("dbus-proxy", "dbus-proxy", m_socket.c_str(), typeString(),
			configPath.c_str(), NULL);
	} else {
		if (m_pid == -1)
			log_error("Failed to spawn DBus proxy!");
	}
}

DBusGateway::~DBusGateway()
{
	if (kill(m_pid, SIGTERM) == -1) {
		log_error("Failed to kill %s proxy!", typeString());
	} else {
		log_debug("Killed %s proxy!", typeString());
	}

	if (remove(m_socket.c_str()) == -1) {
		log_error("Failed to remove %s proxy socket!", typeString());
	} else {
		log_debug("Removed %s proxy socket!", typeString());
	}
}

std::string DBusGateway::id()
{
	return "dbus-proxy";
}

bool DBusGateway::setConfig(const std::string &config)
{
	return true;
}

bool DBusGateway::activate()
{
	return true;
}

const char *DBusGateway::typeString()
{
	if (m_type == SessionProxy) {
		return "session";
	} else {
		return "system";
	}
}

const char *DBusGateway::socketName()
{
	// Return the filename after stripping directory info
	std::string socket(m_socket.c_str());
	return socket.substr(socket.rfind('/') + 1).c_str();
}

std::string DBusGateway::environment()
{
	log_debug("Requesting environment for %s with socket %s",
		typeString(), m_socket.c_str());

	std::string env;
	env += "DBUS_";
	if (m_type == SessionProxy) {
		env += "SESSION";
	} else {
		env += "SYSTEM";
	}
	env += "_BUS_ADDRESS=unix:path=";
	env += "/deployed_app/";
	env += socketName();

	return env;
}