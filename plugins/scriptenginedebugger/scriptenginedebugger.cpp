/*
  scriptenginedebugger.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "scriptenginedebugger.h"

#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"
#include "include/singlecolumnobjectproxymodel.h"

#include <QScriptEngine>
#include <QDebug>
#include <QtPlugin>

using namespace GammaRay;

///NOTE: for crashes related to script engine debugger on shutdown, see:
///      https://bugreports.qt.nokia.com/browse/QTBUG-21548
///      Also it seems that we get another crash when the interrupt action
///      was triggered and we close the mainwindow.

ScriptEngineDebugger::ScriptEngineDebugger(ProbeInterface *probe, QObject *parent)
  : QObject(parent)
{
  ObjectTypeFilterProxyModel<QScriptEngine> *scriptEngineFilter =
    new ObjectTypeFilterProxyModel<QScriptEngine>(this);
  scriptEngineFilter->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy =
    new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(scriptEngineFilter);

  probe->registerModel("com.kdab.GammaRay.ScriptEngines", singleColumnProxy);
}

ScriptEngineDebugger::~ScriptEngineDebugger()
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ScriptEngineDebuggerFactory)
#endif
