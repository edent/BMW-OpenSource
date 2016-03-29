/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"


#include "MarkStack.h"

#include <unistd.h>
#if defined (__SYMBIAN32__)
#include "wtf/FastMalloc.h"
#include <e32base.h>
#include <e32std.h>
#include <e32hal.h>
#include <hal.h>
#else
#include <sys/mman.h>
#endif

namespace JSC {

void MarkStack::initializePagesize()
{
#if defined (__SYMBIAN32__)
    TInt page_size;
    UserHal::PageSizeInBytes(page_size);
    MarkStack::s_pageSize = page_size;
#else
    MarkStack::s_pageSize = getpagesize();
#endif
}

void* MarkStack::allocateStack(size_t size)
{
#if defined (__SYMBIAN32__)
    return fastMalloc(size);
#else
    return mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
#endif
}
void MarkStack::releaseStack(void* addr, size_t size)
{
#if defined (__SYMBIAN32__)
    fastFree(addr);
#else
    munmap(reinterpret_cast<char*>(addr), size);
#endif
}

}
