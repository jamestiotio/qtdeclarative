// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QV4FUNCTION_H
#define QV4FUNCTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qqmlprivate.h>
#include "qv4global_p.h"
#include <private/qv4executablecompilationunit_p.h>
#include <private/qv4context_p.h>
#include <private/qv4string_p.h>

namespace JSC {
class MacroAssemblerCodeRef;
}

QT_BEGIN_NAMESPACE

struct QQmlSourceLocation;

namespace QV4 {

struct Q_QML_EXPORT FunctionData {
    CompiledData::CompilationUnitBase *compilationUnit;

    // Intentionally require an ExecutableCompilationUnit but save only a pointer to
    // CompilationUnitBase. This is so that we can take advantage of the standard layout
    // of CompilationUnitBase in the JIT. Furthermore we can safely static_cast to
    // ExecutableCompilationUnit where we need it.
    FunctionData(ExecutableCompilationUnit *compilationUnit)
        : compilationUnit(compilationUnit)
    {}
};
// Make sure this class can be accessed through offsetof (done by the assemblers):
Q_STATIC_ASSERT(std::is_standard_layout< FunctionData >::value);

struct Q_QML_EXPORT Function : public FunctionData {
protected:
    Function(ExecutionEngine *engine, ExecutableCompilationUnit *unit,
             const CompiledData::Function *function, const QQmlPrivate::AOTCompiledFunction *aotFunction);
    ~Function();

public:
    struct JSTypedFunction {
        QList<QQmlType> argumentTypes;
        QQmlType returnType;
    };

    const CompiledData::Function *compiledFunction;

    QV4::ExecutableCompilationUnit *executableCompilationUnit() const
    {
        // This is safe: We require an ExecutableCompilationUnit in the ctor.
        return static_cast<QV4::ExecutableCompilationUnit *>(compilationUnit);
    }

    QV4::Heap::String *runtimeString(uint i) const
    {
        return compilationUnit->runtimeStrings[i];
    }

    bool call(QObject *thisObject, void **a, const QMetaType *types, int argc,
              ExecutionContext *context);
    ReturnedValue call(const Value *thisObject, const Value *argv, int argc,
                       ExecutionContext *context);

    const char *codeData;

    typedef ReturnedValue (*JittedCode)(CppStackFrame *, ExecutionEngine *);
    JittedCode jittedCode;
    JSC::MacroAssemblerCodeRef *codeRef;
    union {
        const QQmlPrivate::AOTCompiledFunction *aotCompiledFunction = nullptr;
        const JSTypedFunction *jsTypedFunction;
    };

    // first nArguments names in internalClass are the actual arguments
    Heap::InternalClass *internalClass;
    int interpreterCallCount = 0;
    quint16 nFormals;
    enum Kind : quint8 { JsUntyped, JsTyped, AotCompiled, Eval };
    Kind kind = JsUntyped;
    bool detectedInjectedParameters = false;

    static Function *create(ExecutionEngine *engine, ExecutableCompilationUnit *unit,
                            const CompiledData::Function *function,
                            const QQmlPrivate::AOTCompiledFunction *aotFunction);
    void destroy();

    // used when dynamically assigning signal handlers (QQmlConnection)
    void updateInternalClass(ExecutionEngine *engine, const QList<QByteArray> &parameters);

    inline Heap::String *name() const {
        return runtimeString(compiledFunction->nameIndex);
    }

    static QString prettyName(const Function *function, const void *address);

    inline QString sourceFile() const { return executableCompilationUnit()->fileName(); }
    inline QUrl finalUrl() const { return executableCompilationUnit()->finalUrl(); }

    inline bool isStrict() const { return compiledFunction->flags & CompiledData::Function::IsStrict; }
    inline bool isArrowFunction() const { return compiledFunction->flags & CompiledData::Function::IsArrowFunction; }
    inline bool isGenerator() const { return compiledFunction->flags & CompiledData::Function::IsGenerator; }
    inline bool isClosureWrapper() const { return compiledFunction->flags & CompiledData::Function::IsClosureWrapper; }

    QQmlSourceLocation sourceLocation() const;

    Function *nestedFunction() const
    {
        if (compiledFunction->nestedFunctionIndex == std::numeric_limits<uint32_t>::max())
            return nullptr;
        return executableCompilationUnit()->runtimeFunctions[compiledFunction->nestedFunctionIndex];
    }
};

}

QT_END_NAMESPACE

#endif
