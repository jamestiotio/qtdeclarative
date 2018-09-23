/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QV4INTERNALCLASS_H
#define QV4INTERNALCLASS_H

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

#include "qv4global_p.h"

#include <QHash>
#include <private/qv4propertykey_p.h>
#include <private/qv4heap_p.h>

QT_BEGIN_NAMESPACE

namespace QV4 {

struct VTable;
struct MarkStack;

struct PropertyHashData;
struct PropertyHash
{
    struct Entry {
        PropertyKey identifier;
        uint index;
    };

    PropertyHashData *d;

    inline PropertyHash();
    inline PropertyHash(const PropertyHash &other);
    inline ~PropertyHash();
    PropertyHash &operator=(const PropertyHash &other);

    void addEntry(const Entry &entry, int classSize);
    Entry *lookup(PropertyKey identifier) const;
    int removeIdentifier(PropertyKey identifier, int classSize);
    void detach(bool grow, int classSize);
};

struct PropertyHashData
{
    PropertyHashData(int numBits);
    ~PropertyHashData() {
        free(entries);
    }

    int refCount;
    int alloc;
    int size;
    int numBits;
    PropertyHash::Entry *entries;
};

inline PropertyHash::PropertyHash()
{
    d = new PropertyHashData(3);
}

inline PropertyHash::PropertyHash(const PropertyHash &other)
{
    d = other.d;
    ++d->refCount;
}

inline PropertyHash::~PropertyHash()
{
    if (!--d->refCount)
        delete d;
}

inline PropertyHash &PropertyHash::operator=(const PropertyHash &other)
{
    ++other.d->refCount;
    if (!--d->refCount)
        delete d;
    d = other.d;
    return *this;
}



inline PropertyHash::Entry *PropertyHash::lookup(PropertyKey identifier) const
{
    Q_ASSERT(d->entries);

    uint idx = identifier.id() % d->alloc;
    while (1) {
        if (d->entries[idx].identifier == identifier)
            return d->entries + idx;
        if (!d->entries[idx].identifier.isValid())
            return nullptr;
        ++idx;
        idx %= d->alloc;
    }
}

template<typename T>
struct SharedInternalClassDataPrivate {
    SharedInternalClassDataPrivate(ExecutionEngine *)
        : refcount(1),
          m_alloc(0),
          m_size(0),
          data(nullptr)
    { }
    SharedInternalClassDataPrivate(const SharedInternalClassDataPrivate &other)
        : refcount(1),
          m_alloc(other.m_alloc),
          m_size(other.m_size)
    {
        if (m_alloc) {
            data = new T[m_alloc];
            memcpy(data, other.data, m_size*sizeof(T));
        }
    }
    SharedInternalClassDataPrivate(const SharedInternalClassDataPrivate &other, uint pos, T value)
        : refcount(1),
          m_alloc(pos + 8),
          m_size(pos + 1)
    {
        data = new T[m_alloc];
        if (other.data)
            memcpy(data, other.data, (m_size - 1)*sizeof(T));
        data[pos] = value;
    }
    ~SharedInternalClassDataPrivate() { delete [] data; }


    void grow() {
        if (!m_alloc)
            m_alloc = 4;
        T *n = new T[m_alloc * 2];
        if (data) {
            memcpy(n, data, m_alloc*sizeof(T));
            delete [] data;
        }
        data = n;
        m_alloc *= 2;
    }

    uint alloc() const { return m_alloc; }
    uint size() const { return m_size; }
    void setSize(uint s) { m_size = s; }

    T at(uint i) { Q_ASSERT(data && i < m_alloc); return data[i]; }
    void set(uint i, T t) { Q_ASSERT(data && i < m_alloc); data[i] = t; }

    void mark(MarkStack *) {}

    int refcount = 1;
private:
    uint m_alloc;
    uint m_size;
    T *data;
};

template<>
struct SharedInternalClassDataPrivate<PropertyKey> {
    SharedInternalClassDataPrivate(ExecutionEngine *e) : refcount(1), engine(e), data(nullptr) {}
    SharedInternalClassDataPrivate(const SharedInternalClassDataPrivate &other);
    SharedInternalClassDataPrivate(const SharedInternalClassDataPrivate &other, uint pos, PropertyKey value);
    ~SharedInternalClassDataPrivate() {}

    void grow();
    uint alloc() const;
    uint size() const;
    void setSize(uint s);

    PropertyKey at(uint i);
    void set(uint i, PropertyKey t);

    void mark(MarkStack *s);

    int refcount = 1;
private:
    ExecutionEngine *engine;
    Heap::MemberData *data;
};

template <typename T>
struct SharedInternalClassData {
    using Private = SharedInternalClassDataPrivate<T>;
    Private *d;

    inline SharedInternalClassData(ExecutionEngine *e) {
        d = new Private(e);
    }

    inline SharedInternalClassData(const SharedInternalClassData &other)
        : d(other.d)
    {
        ++d->refcount;
    }
    inline ~SharedInternalClassData() {
        if (!--d->refcount)
            delete d;
    }
    SharedInternalClassData &operator=(const SharedInternalClassData &other) {
        ++other.d->refcount;
        if (!--d->refcount)
            delete d;
        d = other.d;
        return *this;
    }

    void add(uint pos, T value) {
        if (pos < d->size()) {
            Q_ASSERT(d->refcount > 1);
            // need to detach
            Private *dd = new Private(*d, pos, value);
            if (!--d->refcount)
                delete d;
            d = dd;
            return;
        }
        Q_ASSERT(pos == d->size());
        if (pos == d->alloc())
            d->grow();
        d->setSize(d->size() + 1);
        d->set(pos, value);
    }

    void set(uint pos, T value) {
        Q_ASSERT(pos < d->size());
        if (d->refcount > 1) {
            // need to detach
            Private *dd = new Private(*d);
            if (!--d->refcount)
                delete d;
            d = dd;
        }
        d->set(pos, value);
    }

    T at(uint i) const {
        Q_ASSERT(i < d->size());
        return d->at(i);
    }
    T operator[] (uint i) {
        Q_ASSERT(i < d->size());
        return d->at(i);
    }

    void mark(MarkStack *s) { d->mark(s); }
};

struct InternalClassTransition
{
    union {
        PropertyKey id;
        const VTable *vtable;
        Heap::Object *prototype;
    };
    Heap::InternalClass *lookup;
    int flags;
    enum {
        // range 0-0xff is reserved for attribute changes
        NotExtensible = 0x100,
        VTableChange = 0x200,
        PrototypeChange = 0x201,
        ProtoClass = 0x202,
        Sealed = 0x203,
        Frozen = 0x204,
        RemoveMember = -1
    };

    bool operator==(const InternalClassTransition &other) const
    { return id == other.id && flags == other.flags; }

    bool operator<(const InternalClassTransition &other) const
    { return id < other.id || (id == other.id && flags < other.flags); }
};

namespace Heap {

struct InternalClass : Base {
    ExecutionEngine *engine;
    const VTable *vtable;
    quintptr protoId; // unique across the engine, gets changed whenever the proto chain changes
    Heap::Object *prototype;
    InternalClass *parent;

    PropertyHash propertyTable; // id to valueIndex
    SharedInternalClassData<PropertyKey> nameMap;
    SharedInternalClassData<PropertyAttributes> propertyData;

    typedef InternalClassTransition Transition;
    std::vector<Transition> transitions;
    InternalClassTransition &lookupOrInsertTransition(const InternalClassTransition &t);

    uint size;
    bool extensible;
    bool isSealed;
    bool isFrozen;
    bool isUsedAsProto;

    void init(ExecutionEngine *engine);
    void init(InternalClass *other);
    void destroy();

    Q_QML_PRIVATE_EXPORT QString keyAt(uint index) const;
    Q_REQUIRED_RESULT InternalClass *nonExtensible();

    static void addMember(QV4::Object *object, PropertyKey id, PropertyAttributes data, uint *index);
    Q_REQUIRED_RESULT InternalClass *addMember(PropertyKey identifier, PropertyAttributes data, uint *index = nullptr);
    Q_REQUIRED_RESULT InternalClass *changeMember(PropertyKey identifier, PropertyAttributes data, uint *index = nullptr);
    static void changeMember(QV4::Object *object, PropertyKey id, PropertyAttributes data, uint *index = nullptr);
    static void removeMember(QV4::Object *object, PropertyKey identifier);
    PropertyHash::Entry *findEntry(const PropertyKey id)
    {
        Q_ASSERT(id.isStringOrSymbol());

        PropertyHash::Entry *e = propertyTable.lookup(id);
        if (e && e->index < size)
            return e;

        return nullptr;
    }

    struct IndexAndAttribute {
        uint index;
        PropertyAttributes attrs;
        bool isValid() const { return index != UINT_MAX; }
    };

    IndexAndAttribute findValueOrGetter(const PropertyKey id)
    {
        Q_ASSERT(id.isStringOrSymbol());

        PropertyHash::Entry *e = propertyTable.lookup(id);
        if (e && e->index < size)
            return { e->index, propertyData.at(e->index) };

        return { UINT_MAX, Attr_Invalid };
    }

    IndexAndAttribute findValueOrSetter(const PropertyKey id)
    {
        Q_ASSERT(id.isStringOrSymbol());

        PropertyHash::Entry *e = propertyTable.lookup(id);
        if (e && e->index < size) {
            PropertyAttributes a = propertyData.at(e->index);
            return { a.isAccessor() ? e->index + 1 : e->index, a };
        }

        return { UINT_MAX, Attr_Invalid };
    }

    uint indexOfValueOrGetter(const PropertyKey id)
    {
        Q_ASSERT(id.isStringOrSymbol());

        PropertyHash::Entry *e = propertyTable.lookup(id);
        if (e && e->index < size)
            return e->index;

        return UINT_MAX;
    }

    bool verifyIndex(const PropertyKey id, uint index)
    {
        Q_ASSERT(id.isStringOrSymbol());

        PropertyHash::Entry *e = propertyTable.lookup(id);
        if (e && e->index < size)
            return e->index == index;

        return false;
    }

    Q_REQUIRED_RESULT InternalClass *sealed();
    Q_REQUIRED_RESULT InternalClass *frozen();
    Q_REQUIRED_RESULT InternalClass *propertiesFrozen() const;

    Q_REQUIRED_RESULT InternalClass *asProtoClass();

    Q_REQUIRED_RESULT InternalClass *changeVTable(const VTable *vt) {
        if (vtable == vt)
            return this;
        return changeVTableImpl(vt);
    }
    Q_REQUIRED_RESULT InternalClass *changePrototype(Heap::Object *proto) {
        if (prototype == proto)
            return this;
        return changePrototypeImpl(proto);
    }

    void updateProtoUsage(Heap::Object *o);

    static void markObjects(Heap::Base *ic, MarkStack *stack);

private:
    Q_QML_EXPORT InternalClass *changeVTableImpl(const VTable *vt);
    Q_QML_EXPORT InternalClass *changePrototypeImpl(Heap::Object *proto);
    InternalClass *addMemberImpl(PropertyKey identifier, PropertyAttributes data, uint *index);

    void removeChildEntry(InternalClass *child);
    friend struct ExecutionEngine;
};

inline
void Base::markObjects(Base *b, MarkStack *stack)
{
    b->internalClass->mark(stack);
}

}

}

QT_END_NAMESPACE

#endif
