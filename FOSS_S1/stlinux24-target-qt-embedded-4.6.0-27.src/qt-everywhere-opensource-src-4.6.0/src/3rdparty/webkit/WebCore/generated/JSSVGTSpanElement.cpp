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

#include "JSSVGTSpanElement.h"

#include "SVGTSpanElement.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGTSpanElement);

/* Hash table for prototype */

static const HashTableValue JSSVGTSpanElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static JSC_CONST_HASHTABLE HashTable JSSVGTSpanElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGTSpanElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSSVGTSpanElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGTSpanElementPrototype::s_info = { "SVGTSpanElementPrototype", 0, &JSSVGTSpanElementPrototypeTable, 0 };

JSObject* JSSVGTSpanElementPrototype::self(ExecState* exec, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSSVGTSpanElement>(exec, globalObject);
}

const ClassInfo JSSVGTSpanElement::s_info = { "SVGTSpanElement", &JSSVGTextPositioningElement::s_info, 0, 0 };

JSSVGTSpanElement::JSSVGTSpanElement(NonNullPassRefPtr<Structure> structure, JSDOMGlobalObject* globalObject, PassRefPtr<SVGTSpanElement> impl)
    : JSSVGTextPositioningElement(structure, globalObject, impl)
{
}

JSObject* JSSVGTSpanElement::createPrototype(ExecState* exec, JSGlobalObject* globalObject)
{
    return new (exec) JSSVGTSpanElementPrototype(JSSVGTSpanElementPrototype::createStructure(JSSVGTextPositioningElementPrototype::self(exec, globalObject)));
}


}

#endif // ENABLE(SVG)
