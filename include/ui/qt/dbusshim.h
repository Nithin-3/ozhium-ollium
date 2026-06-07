/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * dbusshim.h - QObject DBus shim holding a plain UiHandler
 */
#pragma once

#include <QDBusVirtualObject>

struct UiHandler;

class DBusShim : public QDBusVirtualObject {
public:
	explicit DBusShim(UiHandler * handler, QObject *parent = nullptr);

	bool handleMessage(const QDBusMessage &msg, const QDBusConnection &conn) override;
	QString introspect(const QString &path) const override;

private:
	UiHandler *m_handler;
};
