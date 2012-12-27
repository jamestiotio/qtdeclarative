/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quicktest.h"
#include "quicktestresult_p.h"
#include <QtTest/qtestsystem.h>
#include "qtestoptions_p.h"
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtQuick/qquickview.h>
#include <QtQml/qjsvalue.h>
#include <QtQml/qjsengine.h>
#include <QtGui/qopengl.h>
#include <QtCore/qurl.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qdiriterator.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qtextstream.h>
#include <QtGui/qtextdocument.h>
#include <stdio.h>
#include <QtGui/QGuiApplication>
#include <QtCore/QTranslator>
#include <QtTest/QSignalSpy>

#ifdef QT_QMLTEST_WITH_WIDGETS
#include <QtWidgets/QApplication>
#endif

QT_BEGIN_NAMESPACE

class QTestRootObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool windowShown READ windowShown NOTIFY windowShownChanged)
    Q_PROPERTY(bool hasTestCase READ hasTestCase WRITE setHasTestCase NOTIFY hasTestCaseChanged)
public:
    QTestRootObject(QObject *parent = 0)
        : QObject(parent), hasQuit(false), m_windowShown(false), m_hasTestCase(false)  {}

    bool hasQuit:1;
    bool hasTestCase() const { return m_hasTestCase; }
    void setHasTestCase(bool value) { m_hasTestCase = value; emit hasTestCaseChanged(); }

    bool windowShown() const { return m_windowShown; }
    void setWindowShown(bool value) { m_windowShown = value; emit windowShownChanged(); }

Q_SIGNALS:
    void windowShownChanged();
    void hasTestCaseChanged();

private Q_SLOTS:
    void quit() { hasQuit = true; }

private:
    bool m_windowShown : 1;
    bool m_hasTestCase :1;
};

static inline QString stripQuotes(const QString &s)
{
    if (s.length() >= 2 && s.startsWith(QLatin1Char('"')) && s.endsWith(QLatin1Char('"')))
        return s.mid(1, s.length() - 2);
    else
        return s;
}

void handleCompileErrors(const QFileInfo &fi, QQuickView *view)
{
    // Error compiling the test - flag failure in the log and continue.
    const QList<QQmlError> errors = view->errors();
    QuickTestResult results;
    results.setTestCaseName(fi.baseName());
    results.startLogging();
    results.setFunctionName(QLatin1String("compile"));
    // Verbose warning output of all messages and relevant parameters
    QString message;
    QTextStream str(&message);
    str << "\n  " << QDir::toNativeSeparators(fi.absoluteFilePath()) << " produced "
        << errors.size() << " error(s):\n";
    foreach (const QQmlError &e, errors) {
        str << "    ";
        if (e.url().isLocalFile()) {
            str << QDir::toNativeSeparators(e.url().toLocalFile());
        } else {
            str << e.url().toString();
        }
        if (e.line() > 0)
            str << ':' << e.line() << ',' << e.column();
        str << ": " << e.description() << '\n';
    }
    str << "  Working directory: " << QDir::toNativeSeparators(QDir::current().absolutePath()) << '\n';
    if (QQmlEngine *engine = view->engine()) {
        str << "  View: " << view->metaObject()->className() << ", import paths:\n";
        foreach (const QString &i, engine->importPathList())
            str << "    '" << QDir::toNativeSeparators(i) << "'\n";
        const QStringList pluginPaths = engine->pluginPathList();
        str << "  Plugin paths:\n";
        foreach (const QString &p, pluginPaths)
            str << "    '" << QDir::toNativeSeparators(p) << "'\n";
    }
    qWarning("%s", qPrintable(message));
    // Fail with error 0.
    results.fail(errors.at(0).description(),
                 errors.at(0).url(), errors.at(0).line());
    results.finishTestData();
    results.finishTestDataCleanup();
    results.finishTestFunction();
    results.setFunctionName(QString());
    results.stopLogging();
}

bool qWaitForSignal(QObject *obj, const char* signal, int timeout = 5000)
{
    QSignalSpy spy(obj, signal);
    QElapsedTimer timer;
    timer.start();

    while (!spy.size()) {
        int remaining = timeout - int(timer.elapsed());
        if (remaining <= 0)
            break;
        QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        QTest::qSleep(10);
    }

    return spy.size();
}

int quick_test_main(int argc, char **argv, const char *name, const char *sourceDir)
{
    // Look for QML-specific command-line options.
    //      -import dir         Specify an import directory.
    //      -input dir          Specify the input directory for test cases.
    //      -translation file   Specify the translation file.
    QStringList imports;
    QString testPath;
    QString translationFile;
#ifdef QT_QMLTEST_WITH_WIDGETS
    bool withWidgets = false;
#endif
    int outargc = 1;
    int index = 1;
    while (index < argc) {
        if (strcmp(argv[index], "-import") == 0 && (index + 1) < argc) {
            imports += stripQuotes(QString::fromLocal8Bit(argv[index + 1]));
            index += 2;
        } else if (strcmp(argv[index], "-input") == 0 && (index + 1) < argc) {
            testPath = stripQuotes(QString::fromLocal8Bit(argv[index + 1]));
            index += 2;
        } else if (strcmp(argv[index], "-opengl") == 0) {
            ++index;
#ifdef QT_QMLTEST_WITH_WIDGETS
        } else if (strcmp(argv[index], "-widgets") == 0) {
            withWidgets = true;
            ++index;
#endif
        } else if (strcmp(argv[index], "-translation") == 0 && (index + 1) < argc) {
            translationFile = stripQuotes(QString::fromLocal8Bit(argv[index + 1]));
            index += 2;
        } else if (outargc != index) {
            argv[outargc++] = argv[index++];
        } else {
            ++outargc;
            ++index;
        }
    }
    argv[outargc] = 0;
    argc = outargc;

    QCoreApplication* app = 0;
    if (!QCoreApplication::instance()) {
#ifdef QT_QMLTEST_WITH_WIDGETS
        if (withWidgets)
            app = new QApplication(argc, argv);
        else
#endif
        {
            app = new QGuiApplication(argc, argv);
        }
    }

    // Parse the command-line arguments.

    // Setting currentAppname and currentTestObjectName (via setProgramName) are needed
    // for the code coverage analysis. Must be done before parseArgs is called.
    QuickTestResult::setCurrentAppname(argv[0]);
    QuickTestResult::setProgramName(name);

    QuickTestResult::parseArgs(argc, argv);

#ifndef QT_NO_TRANSLATION
    QTranslator translator;
    if (!translationFile.isEmpty()) {
        if (translator.load(translationFile)) {
            app->installTranslator(&translator);
        } else {
            qWarning("Could not load the translation file '%s'.", qPrintable(translationFile));
        }
    }
#endif

    // Determine where to look for the test data.
    if (testPath.isEmpty() && sourceDir) {
        const QString s = QString::fromLocal8Bit(sourceDir);
        if (QFile::exists(s))
            testPath = s;
    }
    if (testPath.isEmpty()) {
        QDir current = QDir::current();
#ifdef Q_OS_WIN
        // Skip release/debug subfolders
        if (!current.dirName().compare(QLatin1String("Release"), Qt::CaseInsensitive)
            || !current.dirName().compare(QLatin1String("Debug"), Qt::CaseInsensitive))
            current.cdUp();
#endif // Q_OS_WIN
        testPath = current.absolutePath();
    }
    QStringList files;

    const QFileInfo testPathInfo(testPath);
    if (testPathInfo.isFile()) {
        if (!testPath.endsWith(QStringLiteral(".qml"))) {
            qWarning("'%s' does not have the suffix '.qml'.", qPrintable(testPath));
            return 1;
        }
        files << testPath;
    } else if (testPathInfo.isDir()) {
        // Scan the test data directory recursively, looking for "tst_*.qml" files.
        const QStringList filters(QStringLiteral("tst_*.qml"));
        QDirIterator iter(testPathInfo.absoluteFilePath(), filters, QDir::Files,
                          QDirIterator::Subdirectories |
                          QDirIterator::FollowSymlinks);
        while (iter.hasNext())
            files += iter.next();
        files.sort();
        if (files.isEmpty()) {
            qWarning("The directory '%s' does not contain any test files matching '%s'",
                     qPrintable(testPath), qPrintable(filters.front()));
            return 1;
        }
    } else {
        qWarning("'%s' does not exist under '%s'.",
                 qPrintable(testPath), qPrintable(QDir::currentPath()));
        return 1;
    }

    // Scan through all of the "tst_*.qml" files and run each of them
    // in turn with a QQuickView.
    QQuickView *view = new QQuickView;
    view->setFlags(Qt::Window | Qt::WindowSystemMenuHint
                         | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint
                         | Qt::WindowCloseButtonHint);
    QTestRootObject rootobj;
    QEventLoop eventLoop;
    QObject::connect(view->engine(), SIGNAL(quit()),
                     &rootobj, SLOT(quit()));
    QObject::connect(view->engine(), SIGNAL(quit()),
                     &eventLoop, SLOT(quit()));
    view->rootContext()->setContextProperty
        (QLatin1String("qtest"), &rootobj);
    foreach (const QString &path, imports)
        view->engine()->addImportPath(path);
    foreach (const QString &file, files) {
        const QFileInfo fi(file);
        if (!fi.exists())
            continue;

        view->setObjectName(fi.baseName());
        view->setTitle(view->objectName());
        rootobj.setHasTestCase(false);
        rootobj.setWindowShown(false);
        rootobj.hasQuit = false;
        QString path = fi.absoluteFilePath();
        if (path.startsWith(QLatin1String(":/")))
            view->setSource(QUrl(QLatin1String("qrc:") + path.mid(2)));
        else
            view->setSource(QUrl::fromLocalFile(path));

        if (QTest::printAvailableFunctions)
            continue;
        if (view->status() == QQuickView::Error) {
            handleCompileErrors(fi, view);
            continue;
        }
        if (!rootobj.hasQuit) {
            // If the test already quit, then it was performed
            // synchronously during setSource().  Otherwise it is
            // an asynchronous test and we need to show the window
            // and wait for the first frame to be rendered
            // and then wait for quit indication.
            view->setFramePosition(QPoint(50, 50));
            if (view->size().isEmpty()) { // Avoid hangs with empty windows.
                qWarning().nospace()
                    << "Test '" << QDir::toNativeSeparators(path) << "' has invalid size "
                    << view->size() << ", resizing.";
                view->resize(200, 200);
            }
            view->show();
            if (qWaitForSignal(view, SIGNAL(frameSwapped())))
                rootobj.setWindowShown(true);
            if (!rootobj.hasQuit && rootobj.hasTestCase())
                eventLoop.exec();
            // view->hide(); Causes a crash in Qt3D due to deletion of the GL context, see QTBUG-27696
        }
    }

    // Flush the current logging stream.
    QuickTestResult::setProgramName(0);
    delete view;
    delete app;

    // Return the number of failures as the exit code.
    return QuickTestResult::exitCode();
}

QT_END_NAMESPACE

#include "quicktest.moc"
