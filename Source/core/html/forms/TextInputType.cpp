/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/forms/TextInputType.h"

#include "core/InputTypeNames.h"
#include "core/html/HTMLInputElement.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

using namespace HTMLNames;

PassRefPtrWillBeRawPtr<InputType> TextInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new TextInputType(element));
}

void TextInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeText);
    if (element().fastHasAttribute(maxlengthAttr))
        countUsageIfVisible(UseCounter::InputTypeTextMaxLength);
    const AtomicString& type = element().fastGetAttribute(typeAttr);
    if (equalIgnoringCase(type, InputTypeNames::datetime))
        countUsageIfVisible(UseCounter::InputTypeDateTimeFallback);
    else if (equalIgnoringCase(type, InputTypeNames::week))
        countUsageIfVisible(UseCounter::InputTypeWeekFallback);
}

const AtomicString& TextInputType::formControlType() const
{
    return InputTypeNames::text;
}

bool TextInputType::supportsInputModeAttribute() const
{
    return true;
}

} // namespace blink
