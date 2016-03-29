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

#include "config.h"

#if ENABLE(SVG) && ENABLE(FILTERS)

#include "JSSVGFETurbulenceElement.h"

#include "CSSMutableStyleDeclaration.h"
#include "CSSStyleDeclaration.h"
#include "CSSValue.h"
#include "JSCSSStyleDeclaration.h"
#include "JSCSSValue.h"
#include "JSSVGAnimatedEnumeration.h"
#include "JSSVGAnimatedInteger.h"
#include "JSSVGAnimatedLength.h"
#include "JSSVGAnimatedNumber.h"
#include "JSSVGAnimatedString.h"
#include "SVGFETurbulenceElement.h"
#include <runtime/Error.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGFETurbulenceElement);

/* Hash table */

static const HashTableValue JSSVGFETurbulenceElementTableValues[15] =
{
    { "baseFrequencyX", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementBaseFrequencyX, (intptr_t)0 },
    { "baseFrequencyY", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementBaseFrequencyY, (intptr_t)0 },
    { "numOctaves", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementNumOctaves, (intptr_t)0 },
    { "seed", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSeed, (intptr_t)0 },
    { "stitchTiles", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementStitchTiles, (intptr_t)0 },
    { "type", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementType, (intptr_t)0 },
    { "x", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementX, (intptr_t)0 },
    { "y", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementY, (intptr_t)0 },
    { "width", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementWidth, (intptr_t)0 },
    { "height", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementHeight, (intptr_t)0 },
    { "result", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementResult, (intptr_t)0 },
    { "className", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementClassName, (intptr_t)0 },
    { "style", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementStyle, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsSVGFETurbulenceElementConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGFETurbulenceElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 255, JSSVGFETurbulenceElementTableValues, 0 };
#else
    { 35, 31, JSSVGFETurbulenceElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGFETurbulenceElementConstructorTableValues[7] =
{
    { "SVG_TURBULENCE_TYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_TURBULENCE_TYPE_FRACTALNOISE", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_FRACTALNOISE, (intptr_t)0 },
    { "SVG_TURBULENCE_TYPE_TURBULENCE", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_TURBULENCE, (intptr_t)0 },
    { "SVG_STITCHTYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_STITCHTYPE_STITCH", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_STITCH, (intptr_t)0 },
    { "SVG_STITCHTYPE_NOSTITCH", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_NOSTITCH, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGFETurbulenceElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 127, JSSVGFETurbulenceElementConstructorTableValues, 0 };
#else
    { 18, 15, JSSVGFETurbulenceElementConstructorTableValues, 0 };
#endif

class JSSVGFETurbulenceElementConstructor : public DOMConstructorObject {
public:
    JSSVGFETurbulenceElementConstructor(ExecState* exec, JSDOMGlobalObject* globalObject)
        : DOMConstructorObject(JSSVGFETurbulenceElementConstructor::createStructure(globalObject->objectPrototype()), globalObject)
    {
        putDirect(exec->propertyNames().prototype, JSSVGFETurbulenceElementPrototype::self(exec, globalObject), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValue proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, StructureFlags)); 
    }
    
protected:
    static const unsigned StructureFlags = OverridesGetOwnPropertySlot | ImplementsHasInstance | DOMConstructorObject::StructureFlags;
};

const ClassInfo JSSVGFETurbulenceElementConstructor::s_info = { "SVGFETurbulenceElementConstructor", 0, &JSSVGFETurbulenceElementConstructorTable, 0 };

bool JSSVGFETurbulenceElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGFETurbulenceElementConstructor, DOMObject>(exec, &JSSVGFETurbulenceElementConstructorTable, this, propertyName, slot);
}

bool JSSVGFETurbulenceElementConstructor::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGFETurbulenceElementConstructor, DOMObject>(exec, &JSSVGFETurbulenceElementConstructorTable, this, propertyName, descriptor);
}

/* Hash table for prototype */

static const HashTableValue JSSVGFETurbulenceElementPrototypeTableValues[8] =
{
    { "SVG_TURBULENCE_TYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_TURBULENCE_TYPE_FRACTALNOISE", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_FRACTALNOISE, (intptr_t)0 },
    { "SVG_TURBULENCE_TYPE_TURBULENCE", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_TURBULENCE, (intptr_t)0 },
    { "SVG_STITCHTYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_STITCHTYPE_STITCH", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_STITCH, (intptr_t)0 },
    { "SVG_STITCHTYPE_NOSTITCH", DontDelete|ReadOnly, (intptr_t)jsSVGFETurbulenceElementSVG_STITCHTYPE_NOSTITCH, (intptr_t)0 },
    { "getPresentationAttribute", DontDelete|Function, (intptr_t)jsSVGFETurbulenceElementPrototypeFunctionGetPresentationAttribute, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGFETurbulenceElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 127, JSSVGFETurbulenceElementPrototypeTableValues, 0 };
#else
    { 18, 15, JSSVGFETurbulenceElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGFETurbulenceElementPrototype::s_info = { "SVGFETurbulenceElementPrototype", 0, &JSSVGFETurbulenceElementPrototypeTable, 0 };

JSObject* JSSVGFETurbulenceElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGFETurbulenceElement>(exec, globalObject);
}

bool JSSVGFETurbulenceElementPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticPropertySlot<JSSVGFETurbulenceElementPrototype, JSObject>(exec, &JSSVGFETurbulenceElementPrototypeTable, this, propertyName, slot);
}

bool JSSVGFETurbulenceElementPrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticPropertyDescriptor<JSSVGFETurbulenceElementPrototype, JSObject>(exec, &JSSVGFETurbulenceElementPrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSSVGFETurbulenceElement::s_info = { "SVGFETurbulenceElement", &JSSVGElement::s_info, &JSSVGFETurbulenceElementTable, 0 };

JSSVGFETurbulenceElement::JSSVGFETurbulenceElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGFETurbulenceElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

JSObject* JSSVGFETurbulenceElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGFETurbulenceElementPrototype(JSSVGFETurbulenceElementPrototype::createStructure(JSSVGElementPrototype::self(exec, globalObject)));
}

bool JSSVGFETurbulenceElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGFETurbulenceElement, Base>(exec, &JSSVGFETurbulenceElementTable, this, propertyName, slot);
}

bool JSSVGFETurbulenceElement::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGFETurbulenceElement, Base>(exec, &JSSVGFETurbulenceElementTable, this, propertyName, descriptor);
}

JSValue jsSVGFETurbulenceElementBaseFrequencyX(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedNumber> obj = imp->baseFrequencyXAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementBaseFrequencyY(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedNumber> obj = imp->baseFrequencyYAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementNumOctaves(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedInteger> obj = imp->numOctavesAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementSeed(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedNumber> obj = imp->seedAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementStitchTiles(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedEnumeration> obj = imp->stitchTilesAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementType(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedEnumeration> obj = imp->typeAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementX(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedLength> obj = imp->xAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementY(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedLength> obj = imp->yAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementWidth(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedLength> obj = imp->widthAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementHeight(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedLength> obj = imp->heightAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementResult(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->resultAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementClassName(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->classNameAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGFETurbulenceElementStyle(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* castedThis = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->style()));
}

JSValue jsSVGFETurbulenceElementConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGFETurbulenceElement* domObject = static_cast<JSSVGFETurbulenceElement*>(asObject(slot.slotBase()));
    return JSSVGFETurbulenceElement::getConstructor(exec, domObject->globalObject());
}
JSValue JSSVGFETurbulenceElement::getConstructor(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSSVGFETurbulenceElementConstructor>(exec, static_cast<JSDOMGlobalObject*>(globalObject));
}

JSValue JSC_HOST_CALL jsSVGFETurbulenceElementPrototypeFunctionGetPresentationAttribute(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGFETurbulenceElement::s_info))
        return throwError(exec, TypeError);
    JSSVGFETurbulenceElement* castedThisObj = static_cast<JSSVGFETurbulenceElement*>(asObject(thisValue));
    SVGFETurbulenceElement* imp = static_cast<SVGFETurbulenceElement*>(castedThisObj->impl());
    const UString& name = args.at(0).toString(exec);


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), WTF::getPtr(imp->getPresentationAttribute(name)));
    return result;
}

// Constant getters

JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_UNKNOWN(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(0));
}

JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_FRACTALNOISE(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(1));
}

JSValue jsSVGFETurbulenceElementSVG_TURBULENCE_TYPE_TURBULENCE(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(2));
}

JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_UNKNOWN(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(0));
}

JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_STITCH(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(1));
}

JSValue jsSVGFETurbulenceElementSVG_STITCHTYPE_NOSTITCH(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(2));
}


}

#endif // ENABLE(SVG) && ENABLE(FILTERS)
