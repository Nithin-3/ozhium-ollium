/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * dbus.h - Plain UI handler (no QObject)
 */
#pragma once

#include <QString>

class QQmlContext;
class QTimer;

struct UiHandler {
	void setTarget(QQmlContext *ctx, QTimer *timer, int timeoutMs);
	void updateUI(int element, int action,
		      double min, double max, double current,
		      const QString &text);

	QQmlContext *m_ctx = nullptr;
	QTimer *m_timer = nullptr;
	int m_timeout = 3000;
};
