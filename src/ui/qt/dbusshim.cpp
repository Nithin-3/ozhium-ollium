/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * dbusshim.cpp - DBusShim dispatches messages to UiHandler
 */

#include "ui/qt/dbusshim.h"
#include "ui/qt/dbus.h"
#include <QDBusConnection>
#include <QDBusMessage>

DBusShim::DBusShim(UiHandler *handler, QObject *parent)
	: QDBusVirtualObject(parent), m_handler(handler) {}

bool DBusShim::handleMessage(const QDBusMessage &msg, const QDBusConnection &conn) {
	if (msg.member() != QLatin1String("updateUI"))
		return false;

	const auto &args = msg.arguments();
	if (args.size() < 6) {
		conn.send(msg.createErrorReply(QDBusError::InvalidArgs,
			"Expected 6 arguments"));
		return true;
	}

	m_handler->updateUI(
		args[0].toInt(),
		args[1].toInt(),
		args[2].toDouble(),
		args[3].toDouble(),
		args[4].toDouble(),
		args[5].toString()
	);

	conn.send(msg.createReply());
	return true;
}

QString DBusShim::introspect(const QString &) const {
	return
		"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
		" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
		"<node name=\"/UiHandler\">\n"
		"  <interface name=\"com.ozhium.ollium.ui\">\n"
		"    <method name=\"updateUI\">\n"
		"      <arg name=\"element\" type=\"i\" direction=\"in\"/>\n"
		"      <arg name=\"action\" type=\"i\" direction=\"in\"/>\n"
		"      <arg name=\"min\" type=\"d\" direction=\"in\"/>\n"
		"      <arg name=\"max\" type=\"d\" direction=\"in\"/>\n"
		"      <arg name=\"current\" type=\"d\" direction=\"in\"/>\n"
		"      <arg name=\"text\" type=\"s\" direction=\"in\"/>\n"
		"    </method>\n"
		"  </interface>\n"
		"</node>\n";
}
