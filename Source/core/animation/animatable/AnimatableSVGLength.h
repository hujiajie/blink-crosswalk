/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef AnimatableSVGLength_h
#define AnimatableSVGLength_h

#include "core/animation/animatable/AnimatableValue.h"
#include "core/svg/SVGLength.h"

namespace blink {

class AnimatableSVGLength final : public AnimatableValue {
public:
    virtual ~AnimatableSVGLength() { }

    static PassRefPtrWillBeRawPtr<AnimatableSVGLength> create(PassRefPtrWillBeRawPtr<SVGLength> length)
    {
        return adoptRefWillBeNoop(new AnimatableSVGLength(length));
    }

    SVGLength* toSVGLength() const
    {
        return m_length.get();
    }

    virtual void trace(Visitor*) override;

protected:
    virtual bool usesDefaultInterpolationWith(const AnimatableValue*) const override;
    virtual PassRefPtrWillBeRawPtr<AnimatableValue> interpolateTo(const AnimatableValue*, double fraction) const override;

private:
    explicit AnimatableSVGLength(PassRefPtrWillBeRawPtr<SVGLength> length)
        : m_length(length)
    {
    }

    virtual AnimatableType type() const override { return TypeSVGLength; }
    virtual bool equalTo(const AnimatableValue*) const override;

    RefPtrWillBeMember<SVGLength> m_length;
};

DEFINE_ANIMATABLE_VALUE_TYPE_CASTS(AnimatableSVGLength, isSVGLength());

} // namespace blink

#endif // AnimatableSVGLength_h
