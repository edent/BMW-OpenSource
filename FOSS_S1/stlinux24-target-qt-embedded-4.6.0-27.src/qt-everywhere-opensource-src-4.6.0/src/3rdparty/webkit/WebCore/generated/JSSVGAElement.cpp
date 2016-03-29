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

#if ENABLE(SVG)

#include "JSSVGAElement.h"

#include "CSSMutableStyleDeclaration.h"
#include "CSSStyleDeclaration.h"
#include "CSSValue.h"
#include "JSCSSStyleDeclaration.h"
#include "JSCSSValue.h"
#include "JSSVGAnimatedBoolean.h"
#include "JSSVGAnimatedString.h"
#include "JSSVGAnimatedTransformList.h"
#include "JSSVGElement.h"
#include "JSSVGMatrix.h"
#include "JSSVGRect.h"
#include "JSSVGStringList.h"
#include "KURL.h"
#include "SVGAElement.h"
#include "SVGElement.h"
#include "SVGStringList.h"
#include <runtime/Error.h>
#include <runtime/JSString.h>
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGAElement);

/* Hash table */

static const HashTableValue JSSVGAElementTableValues[14] =
{
    { "target", DontDelete|ReadOnly, (intptr_t)jsSVGAElementTarget, (intptr_t)0 },
    { "href", DontDelete|ReadOnly, (intptr_t)jsSVGAElementHref, (intptr_t)0 },
    { "requiredFeatures", DontDelete|ReadOnly, (intptr_t)jsSVGAElementRequiredFeatures, (intptr_t)0 },
    { "requiredExtensions", DontDelete|ReadOnly, (intptr_t)jsSVGAElementRequiredExtensions, (intptr_t)0 },
    { "systemLanguage", DontDelete|ReadOnly, (intptr_t)jsSVGAElementSystemLanguage, (intptr_t)0 },
    { "xmllang", DontDelete, (intptr_t)jsSVGAElementXmllang, (intptr_t)setJSSVGAElementXmllang },
    { "xmlspace", DontDelete, (intptr_t)jsSVGAElementXmlspace, (intptr_t)setJSSVGAElementXmlspace },
    { "externalResourcesRequired", DontDelete|ReadOnly, (intptr_t)jsSVGAElementExternalResourcesRequired, (intptr_t)0 },
    { "className", DontDelete|ReadOnly, (intptr_t)jsSVGAElementClassName, (intptr_t)0 },
    { "style", DontDelete|ReadOnly, (intptr_t)jsSVGAElementStyle, (intptr_t)0 },
    { "transform", DontDelete|ReadOnly, (intptr_t)jsSVGAElementTransform, (intptr_t)0 },
    { "nearestViewportElement", DontDelete|ReadOnly, (intptr_t)jsSVGAElementNearestViewportElement, (intptr_t)0 },
    { "farthestViewportElement", DontDelete|ReadOnly, (intptr_t)jsSVGAElementFarthestViewportElement, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGAElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 127, JSSVGAElementTableValues, 0 };
#else
    { 33, 31, JSSVGAElementTableValues, 0 };
#endif

/* Hash table for prototype */

static const HashTableValue JSSVGAElementPrototypeTableValues[7] =
{
    { "hasExtension", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionHasExtension, (intptr_t)1 },
    { "getPresentationAttribute", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionGetPresentationAttribute, (intptr_t)1 },
    { "getBBox", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionGetBBox, (intptr_t)0 },
    { "getCTM", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionGetCTM, (intptr_t)0 },
    { "getScreenCTM", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionGetScreenCTM, (intptr_t)0 },
    { "getTransformToElement", DontDelete|Function, (intptr_t)jsSVGAElementPrototypeFunctionGetTransformToElement, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGAElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 2047, JSSVGAElementPrototypeTableValues, 0 };
#else
    { 17, 15, JSSVGAElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGAElementPrototype::s_info = { "SVGAElementPrototype", 0, &JSSVGAElementPrototypeTable, 0 };

JSObject* JSSVGAElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGAElement>(exec, globalObject);
}

bool JSSVGAElementPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSSVGAElementPrototypeTable, this, propertyName, slot);
}

bool JSSVGAElementPrototype::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticFunctionDescriptor<JSObject>(exec, &JSSVGAElementPrototypeTable, this, propertyName, descriptor);
}

const ClassInfo JSSVGAElement::s_info = { "SVGAElement", &JSSVGElement::s_info, &JSSVGAElementTable, 0 };

JSSVGAElement::JSSVGAElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGAElement> impl)
    : JSSVGElement(structure, globalObject, impl)
{
}

JSObject* JSSVGAElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGAElementPrototype(JSSVGAElementPrototype::createStructure(JSSVGElementPrototype::self(exec, globalObject)));
}

bool JSSVGAElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGAElement, Base>(exec, &JSSVGAElementTable, this, propertyName, slot);
}

bool JSSVGAElement::getOwnPropertyDescriptor(ExecState* exec, const Identifier& propertyName, PropertyDescriptor& descriptor)
{
    return getStaticValueDescriptor<JSSVGAElement, Base>(exec, &JSSVGAElementTable, this, propertyName, descriptor);
}

JSValue jsSVGAElementTarget(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->targetAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGAElementHref(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->hrefAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGAElementRequiredFeatures(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->requiredFeatures()), imp);
}

JSValue jsSVGAElementRequiredExtensions(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->requiredExtensions()), imp);
}

JSValue jsSVGAElementSystemLanguage(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->systemLanguage()), imp);
}

JSValue jsSVGAElementXmllang(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return jsString(exec, imp->xmllang());
}

JSValue jsSVGAElementXmlspace(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return jsString(exec, imp->xmlspace());
}

JSValue jsSVGAElementExternalResourcesRequired(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    RefPtr<SVGAnimatedBoolean> obj = imp->externalResourcesRequiredAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGAElementClassName(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    RefPtr<SVGAnimatedString> obj = imp->classNameAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGAElementStyle(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->style()));
}

JSValue jsSVGAElementTransform(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    RefPtr<SVGAnimatedTransformList> obj = imp->transformAnimated();
    return toJS(exec, castedThis->globalObject(), obj.get(), imp);
}

JSValue jsSVGAElementNearestViewportElement(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->nearestViewportElement()));
}

JSValue jsSVGAElementFarthestViewportElement(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    JSSVGAElement* castedThis = static_cast<JSSVGAElement*>(asObject(slot.slotBase()));
    UNUSED_PARAM(exec);
    SVGAElement* imp = static_cast<SVGAElement*>(castedThis->impl());
    return toJS(exec, castedThis->globalObject(), WTF::getPtr(imp->farthestViewportElement()));
}

void JSSVGAElement::put(ExecState* exec, const Identifier& propertyName, JSValue value, PutPropertySlot& slot)
{
    lookupPut<JSSVGAElement, Base>(exec, propertyName, value, &JSSVGAElementTable, this, slot);
}

void setJSSVGAElementXmllang(ExecState* exec, JSObject* thisObject, JSValue value)
{
    SVGAElement* imp = static_cast<SVGAElement*>(static_cast<JSSVGAElement*>(thisObject)->impl());
    imp->setXmllang(value.toString(exec));
}

void setJSSVGAElementXmlspace(ExecState* exec, JSObject* thisObject, JSValue value)
{
    SVGAElement* imp = static_cast<SVGAElement*>(static_cast<JSSVGAElement*>(thisObject)->impl());
    imp->setXmlspace(value.toString(exec));
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionHasExtension(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());
    const UString& extension = args.at(0).toString(exec);


    JSC::JSValue result = jsBoolean(imp->hasExtension(extension));
    return result;
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionGetPresentationAttribute(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());
    const UString& name = args.at(0).toString(exec);


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), WTF::getPtr(imp->getPresentationAttribute(name)));
    return result;
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionGetBBox(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), JSSVGStaticPODTypeWrapper<FloatRect>::create(imp->getBBox()).get(), imp);
    return result;
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionGetCTM(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), JSSVGStaticPODTypeWrapper<TransformationMatrix>::create(imp->getCTM()).get(), imp);
    return result;
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionGetScreenCTM(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), JSSVGStaticPODTypeWrapper<TransformationMatrix>::create(imp->getScreenCTM()).get(), imp);
    return result;
}

JSValue JSC_HOST_CALL jsSVGAElementPrototypeFunctionGetTransformToElement(ExecState* exec, JSObject*, JSValue thisValue, const ArgList& args)
{
    UNUSED_PARAM(args);
    if (!thisValue.inherits(&JSSVGAElement::s_info))
        return throwError(exec, TypeError);
    JSSVGAElement* castedThisObj = static_cast<JSSVGAElement*>(asObject(thisValue));
    SVGAElement* imp = static_cast<SVGAElement*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    SVGElement* element = toSVGElement(args.at(0));


    JSC::JSValue result = toJS(exec, castedThisObj->globalObject(), JSSVGStaticPODTypeWrapper<TransformationMatrix>::create(imp->getTransformToElement(element, ec)).get(), imp);
    setDOMException(exec, ec);
    return result;
}


}

#endif // ENABLE(SVG)
