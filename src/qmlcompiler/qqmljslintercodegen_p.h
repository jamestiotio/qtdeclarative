/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQMLJSLINTERCODEGEN_P_H
#define QQMLJSLINTERCODEGEN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <QString>
#include <QFile>
#include <QList>

#include <variant>
#include <memory>
#include <private/qqmljsdiagnosticmessage_p.h>
#include <private/qqmlirbuilder_p.h>
#include <private/qqmljsscope_p.h>
#include <private/qqmljscompiler_p.h>

#include <QtQmlCompiler/private/qqmljstyperesolver_p.h>
#include <QtQmlCompiler/private/qqmljslogger_p.h>
#include <QtQmlCompiler/private/qqmljscompilepass_p.h>

QT_BEGIN_NAMESPACE

namespace QQmlSA {
class PassManager;
};

class QQmlJSLinterCodegen : public QQmlJSAotCompiler
{
public:
    QQmlJSLinterCodegen(QQmlJSImporter *importer, const QString &fileName,
                        const QStringList &qmldirFiles, QQmlJSLogger *logger,
                        QQmlJSTypeInfo *typeInfo);

    void setDocument(const QmlIR::JSCodeGen *codegen, const QmlIR::Document *document) override;
    std::variant<QQmlJSAotFunction, QQmlJS::DiagnosticMessage>
    compileBinding(const QV4::Compiler::Context *context, const QmlIR::Binding &irBinding) override;
    std::variant<QQmlJSAotFunction, QQmlJS::DiagnosticMessage>
    compileFunction(const QV4::Compiler::Context *context,
                    const QmlIR::Function &irFunction) override;

    void setTypeResolver(QQmlJSTypeResolver typeResolver)
    {
        m_typeResolver = std::move(typeResolver);
    }

    QQmlJSTypeResolver *typeResolver() { return &m_typeResolver; }

    void setPassManager(QQmlSA::PassManager *passManager) { m_passManager = passManager; }

    QQmlSA::PassManager *passManager() { return m_passManager; }

private:
    QQmlJSTypeInfo *m_typeInfo;
    QQmlSA::PassManager *m_passManager = nullptr;

    bool analyzeFunction(const QV4::Compiler::Context *context,
                         QQmlJSCompilePass::Function *function, QQmlJS::DiagnosticMessage *error);
};

QT_END_NAMESPACE

#endif
