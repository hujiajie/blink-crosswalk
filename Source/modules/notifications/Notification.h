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

#ifndef Notification_h
#define Notification_h

#include "core/dom/ActiveDOMObject.h"
#include "modules/EventTargetModules.h"
#include "platform/AsyncMethodRunner.h"
#include "platform/heap/Handle.h"
#include "platform/text/TextDirection.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/modules/notifications/WebNotificationDelegate.h"
#include "public/platform/modules/notifications/WebNotificationPermission.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class ExecutionContext;
class NotificationOptions;
class NotificationPermissionCallback;
struct WebNotificationData;

class Notification final : public RefCountedGarbageCollectedEventTargetWithInlineData<Notification>, public ActiveDOMObject, public WebNotificationDelegate {
    DEFINE_EVENT_TARGET_REFCOUNTING_WILL_BE_REMOVED(RefCountedGarbageCollected<Notification>);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Notification);
    DEFINE_WRAPPERTYPEINFO();
public:
    // Used for JavaScript instantiations of the Notification object. Will automatically schedule for
    // the notification to be displayed to the user.
    static Notification* create(ExecutionContext*, const String& title, const NotificationOptions&, ExceptionState&);

    // Used for embedder-created Notification objects. Will initialize the Notification's state as showing.
    static Notification* create(ExecutionContext*, const String& persistentId, const WebNotificationData&);

    virtual ~Notification();

    void close();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(click);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(show);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(close);

    // WebNotificationDelegate implementation.
    void dispatchShowEvent() override;
    void dispatchClickEvent() override;
    void dispatchErrorEvent() override;
    void dispatchCloseEvent() override;

    String title() const { return m_title; }
    String dir() const { return m_dir; }
    String lang() const { return m_lang; }
    String body() const { return m_body; }
    String tag() const { return m_tag; }
    String icon() const { return m_iconUrl; }

    TextDirection direction() const;
    KURL iconURL() const { return m_iconUrl; }

    static String permissionString(WebNotificationPermission);
    static String permission(ExecutionContext*);
    static WebNotificationPermission checkPermission(ExecutionContext*);
    static void requestPermission(ExecutionContext*, NotificationPermissionCallback* = nullptr);

    // EventTarget interface.
    virtual ExecutionContext* executionContext() const override final { return ActiveDOMObject::executionContext(); }
    virtual bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>) override final;
    virtual const AtomicString& interfaceName() const override;

    // ActiveDOMObject interface.
    virtual void stop() override;
    virtual bool hasPendingActivity() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    Notification(const String& title, ExecutionContext*);

    void scheduleShow();

    // Calling show() may start asynchronous operation. If this object has
    // a V8 wrapper, hasPendingActivity() prevents the wrapper from being
    // collected while m_state is Showing, and so this instance stays alive
    // until the operation completes. Otherwise, you need to hold a ref on this
    // instance until the operation completes.
    void show();

    void setDir(const String& dir) { m_dir = dir; }
    void setLang(const String& lang) { m_lang = lang; }
    void setBody(const String& body) { m_body = body; }
    void setIconUrl(KURL iconUrl) { m_iconUrl = iconUrl; }
    void setTag(const String& tag) { m_tag = tag; }

    void setPersistentId(const String& persistentId) { m_persistentId = persistentId; }

private:
    String m_title;
    String m_dir;
    String m_lang;
    String m_body;
    String m_tag;

    KURL m_iconUrl;

    // Notifications can either be bound to the page, which means they're identified by
    // their delegate, or persistent, which means they're identified by a persistent Id
    // given to us by the embedder. This influences how we close the notification.
    String m_persistentId;

    enum NotificationState {
        NotificationStateIdle,
        NotificationStateShowing,
        NotificationStateClosing,
        NotificationStateClosed
    };

    // Only to be used by the Notification::create() method when notifications were created
    // by the embedder rather than by Blink.
    void setState(NotificationState state) { m_state = state; }

    NotificationState m_state;

    AsyncMethodRunner<Notification> m_asyncRunner;
};

} // namespace blink

#endif // Notification_h
