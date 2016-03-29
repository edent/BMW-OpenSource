/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef JSCanvasUnsignedByteArray_h
#define JSCanvasUnsignedByteArray_h

#if ENABLE(3D_CANVAS)

#include "CanvasUnsignedByteArray.h"
#include "JSCanvasArray.h"

namespace WebCore {

class CanvasUnsignedByteArray;

class JSCanvasUnsignedByteArray : public JSCanvasArray {
    typedef JSCanvasArray Base;
public:
    JSCanvasUnsignedByteArray(NonNullPassRefPtr<JSC::Structure>, JSDOMGlobalObject*, PassRefPtr<CanvasUnsignedByteArray>);
    static JSC::JSObject* createPrototype(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);
    virtual bool getOwnPropertySlot(JSC::ExecState*, unsigned propertyName, JSC::PropertySlot&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValue, JSC::PutPropertySlot&);
    virtual void put(JSC::ExecState*, unsigned propertyName, JSC::JSValue);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;

    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags));
    }

    virtual void getOwnPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&);
    CanvasUnsignedByteArray* impl() const
    {
        return static_cast<CanvasUnsignedByteArray*>(Base::impl());
    }
protected:
    static const unsigned StructureFlags = JSC::OverridesGetPropertyNames | JSC::OverridesGetOwnPropertySlot | Base::StructureFlags;
    JSC::JSValue getByIndex(JSC::ExecState*, unsigned index);
    void indexSetter(JSC::ExecState*, unsigned index, JSC::JSValue);
};

JSC::JSValue toJS(JSC::ExecState*, JSDOMGlobalObject*, CanvasUnsignedByteArray*);
CanvasUnsignedByteArray* toCanvasUnsignedByteArray(JSC::JSValue);

class JSCanvasUnsignedByteArrayPrototype : public JSC::JSObject {
    typedef JSC::JSObject Base;
public:
    static JSC::JSObject* self(JSC::ExecState*, JSC::JSGlobalObject*);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;
    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValue prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags));
    }
    JSCanvasUnsignedByteArrayPrototype(NonNullPassRefPtr<JSC::Structure> structure) : JSC::JSObject(structure) { }
protected:
    static const unsigned StructureFlags = Base::StructureFlags;
};


} // namespace WebCore

#endif // ENABLE(3D_CANVAS)

#endif
