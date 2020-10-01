/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

#include "qqmljsscope_p.h"

#include <QtCore/qqueue.h>
#include <QtCore/qsharedpointer.h>

#include <algorithm>

QQmlJSScope::QQmlJSScope(ScopeType type, const QQmlJSScope::Ptr &parentScope)
    : m_parentScope(parentScope), m_scopeType(type) {}

QQmlJSScope::Ptr QQmlJSScope::create(ScopeType type, const QQmlJSScope::Ptr &parentScope)
{
    QQmlJSScope::Ptr childScope(new QQmlJSScope{type, parentScope});
    if (parentScope) {
        Q_ASSERT(type != ScopeType::QMLScope
                || !parentScope->m_parentScope
                || parentScope->parentScope()->m_scopeType == ScopeType::QMLScope
                || parentScope->parentScope()->m_internalName == QLatin1String("global"));
        parentScope->m_childScopes.push_back(childScope);
    }
    return childScope;
}

void QQmlJSScope::insertJSIdentifier(const QString &name, const JavaScriptIdentifier &identifier)
{
    Q_ASSERT(m_scopeType != ScopeType::QMLScope);
    if (identifier.kind == JavaScriptIdentifier::LexicalScoped
            || identifier.kind == JavaScriptIdentifier::Injected
            || m_scopeType == ScopeType::JSFunctionScope) {
        m_jsIdentifiers.insert(name, identifier);
    } else {
        auto targetScope = parentScope();
        while (targetScope->m_scopeType != ScopeType::JSFunctionScope)
            targetScope = targetScope->parentScope();
        targetScope->m_jsIdentifiers.insert(name, identifier);
    }
}

void QQmlJSScope::insertPropertyIdentifier(const QQmlJSMetaProperty &property)
{
    addProperty(property);
    QQmlJSMetaMethod method(property.propertyName() + QLatin1String("Changed"), QLatin1String("void"));
    addMethod(method);
}

bool QQmlJSScope::isIdInCurrentScope(const QString &id) const
{
    return isIdInCurrentQMlScopes(id) || isIdInCurrentJSScopes(id);
}

bool QQmlJSScope::isIdInCurrentQMlScopes(const QString &id) const
{
    if (m_scopeType == ScopeType::QMLScope)
        return m_properties.contains(id) || m_methods.contains(id) || m_enums.contains(id);

    const auto qmlScope = findCurrentQMLScope(parentScope());
    return qmlScope->m_properties.contains(id)
            || qmlScope->m_methods.contains(id)
            || qmlScope->m_enums.contains(id);
}

bool QQmlJSScope::isIdInCurrentJSScopes(const QString &id) const
{
    if (m_scopeType != ScopeType::QMLScope && m_jsIdentifiers.contains(id))
        return true;

    for (auto jsScope = parentScope(); jsScope; jsScope = jsScope->parentScope()) {
        if (jsScope->m_scopeType != ScopeType::QMLScope && jsScope->m_jsIdentifiers.contains(id))
            return true;
    }

    return false;
}

bool QQmlJSScope::isIdInjectedFromSignal(const QString &id) const
{
    const auto found = findJSIdentifier(id);
    return found.has_value() && found->kind == JavaScriptIdentifier::Injected;
}

std::optional<JavaScriptIdentifier> QQmlJSScope::findJSIdentifier(const QString &id) const
{
    for (const auto *scope = this; scope; scope = scope->parentScope().data()) {
        if (scope->m_scopeType == ScopeType::JSFunctionScope
                || scope->m_scopeType == ScopeType::JSLexicalScope) {
            auto it = scope->m_jsIdentifiers.find(id);
            if (it != scope->m_jsIdentifiers.end())
                return *it;
        }
    }

    return std::optional<JavaScriptIdentifier>{};
}

void QQmlJSScope::resolveTypes(const QHash<QString, QQmlJSScope::ConstPtr> &contextualTypes)
{
    auto findType = [&](const QString &name) {
        auto type = contextualTypes.constFind(name);
        if (type != contextualTypes.constEnd())
            return *type;

        return QQmlJSScope::ConstPtr();
    };

    m_baseType = findType(m_baseTypeName);
    m_attachedType = findType(m_attachedTypeName);

    for (auto it = m_properties.begin(), end = m_properties.end(); it != end; ++it)
        it->setType(findType(it->typeName()));

    for (auto it = m_methods.begin(), end = m_methods.end(); it != end; ++it) {
        it->setReturnType(findType(it->returnTypeName()));
        const auto paramNames = it->parameterTypeNames();
        QList<QQmlJSScope::ConstPtr> paramTypes;

        for (const QString &paramName: paramNames)
            paramTypes.append(findType(paramName));

        it->setParameterTypes(paramTypes);
    }
}

QQmlJSScope::ConstPtr QQmlJSScope::findCurrentQMLScope(const QQmlJSScope::ConstPtr &scope)
{
    auto qmlScope = scope;
    while (qmlScope && qmlScope->m_scopeType != ScopeType::QMLScope)
        qmlScope = qmlScope->parentScope();
    return qmlScope;
}

void QQmlJSScope::addExport(const QString &name, const QString &package, const QTypeRevision &version)
{
    m_exports.append(Export(package, name, version, 0));
}

void QQmlJSScope::setExportMetaObjectRevision(int exportIndex, int metaObjectRevision)
{
    m_exports[exportIndex].setMetaObjectRevision(metaObjectRevision);
}

QQmlJSScope::Export::Export(QString package, QString type, const QTypeRevision &version,
                          int metaObjectRevision) :
    m_package(std::move(package)),
    m_type(std::move(type)),
    m_version(version),
    m_metaObjectRevision(metaObjectRevision)
{
}

bool QQmlJSScope::Export::isValid() const
{
    return m_version.isValid() || !m_package.isEmpty() || !m_type.isEmpty();
}
