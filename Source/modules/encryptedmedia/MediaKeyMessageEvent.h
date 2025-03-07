/*
 * Copyright (C) 2012 Google Inc.  All rights reserved.
 * Copyright (C) 2013 Apple Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaKeyMessageEvent_h
#define MediaKeyMessageEvent_h

#include "core/html/MediaKeyError.h"
#include "modules/EventModules.h"
#include "modules/encryptedmedia/MediaKeyMessageEventInit.h"

namespace blink {

class DOMArrayBuffer;

class MediaKeyMessageEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~MediaKeyMessageEvent();

    static PassRefPtrWillBeRawPtr<MediaKeyMessageEvent> create()
    {
        return adoptRefWillBeNoop(new MediaKeyMessageEvent);
    }

    static PassRefPtrWillBeRawPtr<MediaKeyMessageEvent> create(const AtomicString& type, const MediaKeyMessageEventInit& initializer)
    {
        return adoptRefWillBeNoop(new MediaKeyMessageEvent(type, initializer));
    }

    virtual const AtomicString& interfaceName() const override;

    String messageType() const { return m_messageType; }
    DOMArrayBuffer* message() const { return m_message.get(); }

    virtual void trace(Visitor*) override;

private:
    MediaKeyMessageEvent();
    MediaKeyMessageEvent(const AtomicString& type, const MediaKeyMessageEventInit& initializer);

    String m_messageType;
    RefPtr<DOMArrayBuffer> m_message;
};

} // namespace blink

#endif // MediaKeyMessageEvent_h
