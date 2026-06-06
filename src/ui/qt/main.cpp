/*
 * Copyright (c) 2026, ozhium-ollium
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * https://github.com/Nithin-3/ozhium-ollium
 *
 * main.cpp - Qt UI entry point
 */

#include "shared/common.h"
#include "ui/qt/args.h"
#include "ui/qt/dbus.h"
#include "ui/qt/dbusshim.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <QUrl>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ini.h"

struct OsdConfig { int timeoutMs = 3000; };

static int configHandler(void *user, const char *section,
                         const char *name, const char *value) {
	auto *cfg = (OsdConfig *)user;
	if (strcmp(section, "osd") == 0 && strcmp(name, "timeout") == 0) {
		int val = atoi(value);
		if (val > 0)
			cfg->timeoutMs = val * 1000;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	ParsedArgs args;
	if (parseArgs(argc, argv, &args) != 0) {
		fprintf(stderr, "Usage: %s --element slider|text [--min f] [--max f] [--current f] " "--action s --text s\n", argv[0]);
		return 1;
	}

	QDBusInterface iface("com.ozhium.ollium.ui", "/UiHandler", "com.ozhium.ollium.ui", QDBusConnection::sessionBus()); if (iface.isValid()) {
		iface.call("updateUI", (int)args.element, (int)args.action, (double)args.min, (double)args.max, (double)args.current, QString::fromUtf8(args.text)); 
		return 0;
	}

	qputenv("QT_QPA_PLATFORM", "xcb");

	QGuiApplication app(argc, argv);

	char *path = findConfigPath("ozhium-ollium.qml");
	if (!path) {
		fprintf(stderr, "error: ozhium-ollium.qml not found\n");
		return 1;
	}

	QQmlApplicationEngine engine;
	engine.connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, [&]() { QGuiApplication::exit(-1); });

	QQmlContext *ctx = engine.rootContext();
	ctx->setContextProperty("argsElement", args.element == SLIDER ? "slider" : "text");
	ctx->setContextProperty("argsAction", QString::number(args.action));
	ctx->setContextProperty("argsMin", args.min);
	ctx->setContextProperty("argsMax", args.max);
	ctx->setContextProperty("argsCurrent", args.current);
	ctx->setContextProperty("argsText", QString::fromUtf8(args.text));

	engine.load(QUrl::fromLocalFile(path));

	if (engine.rootObjects().isEmpty())
		return -1;

	OsdConfig cfg;
	char *confPath = findConfigPath("ozhium-ollium.conf");
	if (confPath)
		ini_parse(confPath, configHandler, &cfg);

	auto *timer = new QTimer(&app);
	timer->setSingleShot(true);
	QObject::connect(timer, &QTimer::timeout, &app, &QGuiApplication::quit);
	timer->start(cfg.timeoutMs);

	UiHandler handler;
	handler.setTarget(ctx, timer, cfg.timeoutMs);

	auto *shim = new DBusShim(&handler, &app);

	QDBusConnection bus = QDBusConnection::sessionBus();
	if (!bus.registerService("com.ozhium.ollium.ui")) {
		fprintf(stderr, "warning: DBus service name taken\n");
	}
	bus.registerObject("/UiHandler", shim);

	return app.exec();
}
