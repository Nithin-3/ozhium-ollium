/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * dbus.cpp - Plain UiHandler implementation
 */

#include "ui/qt/dbus.h"
#include <QQmlContext>
#include <QTimer>

void UiHandler::setTarget(QQmlContext *ctx, QTimer *timer, int timeoutMs) {
	m_ctx = ctx;
	m_timer = timer;
	m_timeout = timeoutMs;
}

void UiHandler::updateUI(int element, int action, double min, double max, double current, const QString &text) {
	if (!m_ctx || !m_timer)
		return;

	m_ctx->setContextProperty("argsElement", element == 0 ? "slider" : "text");
	m_ctx->setContextProperty("argsAction", QString::number(action));
	m_ctx->setContextProperty("argsMin", min);
	m_ctx->setContextProperty("argsMax", max);
	m_ctx->setContextProperty("argsCurrent", current);
	m_ctx->setContextProperty("argsText", text);

	m_timer->start(m_timeout);
}
