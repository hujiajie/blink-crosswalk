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

#ifndef InspectorResourceAgent_h
#define InspectorResourceAgent_h

#include "bindings/core/v8/ScriptString.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Resource;
struct FetchInitiatorInfo;
class Document;
class DocumentLoader;
class ExecutionContext;
class FormData;
class LocalFrame;
class HTTPHeaderMap;
class InspectorFrontend;
class InspectorPageAgent;
class JSONObject;
class KURL;
class NetworkResourcesData;
class ResourceError;
class ResourceLoader;
class ResourceRequest;
class ResourceResponse;
class ThreadableLoaderClient;
class XHRReplayData;
class XMLHttpRequest;

class WebSocketHandshakeRequest;
class WebSocketHandshakeResponse;

typedef String ErrorString;

class InspectorResourceAgent final : public InspectorBaseAgent<InspectorResourceAgent>, public InspectorBackendDispatcher::NetworkCommandHandler {
public:
    static PassOwnPtrWillBeRawPtr<InspectorResourceAgent> create(InspectorPageAgent* pageAgent)
    {
        return adoptPtrWillBeNoop(new InspectorResourceAgent(pageAgent));
    }

    virtual void setFrontend(InspectorFrontend*) override;
    virtual void clearFrontend() override;
    virtual void restore() override;

    virtual ~InspectorResourceAgent();
    DECLARE_VIRTUAL_TRACE();

    // Called from instrumentation.
    void willSendRequest(unsigned long identifier, DocumentLoader*, ResourceRequest&, const ResourceResponse& redirectResponse, const FetchInitiatorInfo&);
    void markResourceAsCached(unsigned long identifier);
    void didReceiveResourceResponse(LocalFrame*, unsigned long identifier, DocumentLoader*, const ResourceResponse&, ResourceLoader*);
    void didReceiveData(LocalFrame*, unsigned long identifier, const char* data, int dataLength, int encodedDataLength);
    void didFinishLoading(unsigned long identifier, DocumentLoader*, double monotonicFinishTime, int64_t encodedDataLength);
    void didReceiveCORSRedirectResponse(LocalFrame*, unsigned long identifier, DocumentLoader*, const ResourceResponse&, ResourceLoader*);
    void didFailLoading(unsigned long identifier, const ResourceError&);
    void didCommitLoad(LocalFrame*, DocumentLoader*);
    void scriptImported(unsigned long identifier, const String& sourceString);
    void didReceiveScriptResponse(unsigned long identifier);
    bool shouldForceCORSPreflight();

    void documentThreadableLoaderStartedLoadingForClient(unsigned long identifier, ThreadableLoaderClient*);
    void willLoadXHR(XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString& method, const KURL&, bool async, PassRefPtr<FormData> body, const HTTPHeaderMap& headers, bool includeCrendentials);
    void didFailXHRLoading(XMLHttpRequest*, ThreadableLoaderClient*);
    void didFinishXHRLoading(ExecutionContext*, XMLHttpRequest*, ThreadableLoaderClient*, unsigned long identifier, ScriptString sourceString, const AtomicString&, const String&);

    void willSendEventSourceRequest(ThreadableLoaderClient*);
    void willDispachEventSourceEvent(ThreadableLoaderClient*, const AtomicString& eventName, const AtomicString& eventId, const Vector<UChar>& data);
    void didFinishEventSourceRequest(ThreadableLoaderClient*);

    void willDestroyResource(Resource*);

    void applyUserAgentOverride(String* userAgent);

    // FIXME: InspectorResourceAgent should not be aware of style recalculation.
    void willRecalculateStyle(Document*);
    void didRecalculateStyle(int);
    void didScheduleStyleRecalculation(Document*);

    void frameScheduledNavigation(LocalFrame*, double);
    void frameClearedScheduledNavigation(LocalFrame*);

    PassRefPtr<TypeBuilder::Network::Initiator> buildInitiatorObject(Document*, const FetchInitiatorInfo&);

    void didCreateWebSocket(Document*, unsigned long identifier, const KURL& requestURL, const String&);
    void willSendWebSocketHandshakeRequest(Document*, unsigned long identifier, const WebSocketHandshakeRequest*);
    void didReceiveWebSocketHandshakeResponse(Document*, unsigned long identifier, const WebSocketHandshakeRequest*, const WebSocketHandshakeResponse*);
    void didCloseWebSocket(Document*, unsigned long identifier);
    void didReceiveWebSocketFrame(unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength);
    void didSendWebSocketFrame(unsigned long identifier, int opCode, bool masked, const char* payload, size_t payloadLength);
    void didReceiveWebSocketFrameError(unsigned long identifier, const String&);

    // Called from frontend
    virtual void enable(ErrorString*) override;
    virtual void disable(ErrorString*) override;
    virtual void setUserAgentOverride(ErrorString*, const String& userAgent) override;
    virtual void setExtraHTTPHeaders(ErrorString*, const RefPtr<JSONObject>&) override;
    virtual void getResponseBody(ErrorString*, const String& requestId, PassRefPtrWillBeRawPtr<GetResponseBodyCallback>) override;

    virtual void replayXHR(ErrorString*, const String& requestId) override;

    virtual void canClearBrowserCache(ErrorString*, bool*) override;
    virtual void canClearBrowserCookies(ErrorString*, bool*) override;
    virtual void emulateNetworkConditions(ErrorString*, bool, double, double, double) override;
    virtual void setCacheDisabled(ErrorString*, bool cacheDisabled) override;

    virtual void loadResourceForFrontend(ErrorString*, const String& url, const RefPtr<JSONObject>* requestHeaders, PassRefPtrWillBeRawPtr<LoadResourceForFrontendCallback>) override;
    virtual void setDataSizeLimitsForTest(ErrorString*, int maxTotal, int maxResource) override;

    // Called from other agents.
    void setHostId(const String&);
    bool fetchResourceContent(Document*, const KURL&, String* content, bool* base64Encoded);

private:
    explicit InspectorResourceAgent(InspectorPageAgent*);

    void enable();
    void delayedRemoveReplayXHR(XMLHttpRequest*);
    void removeFinishedReplayXHRFired(Timer<InspectorResourceAgent>*);

    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
    InspectorFrontend::Network* m_frontend;
    String m_userAgentOverride;
    String m_hostId;
    OwnPtr<NetworkResourcesData> m_resourcesData;

    typedef WillBeHeapHashMap<ThreadableLoaderClient*, RefPtrWillBeMember<XHRReplayData> > PendingXHRReplayDataMap;
    PendingXHRReplayDataMap m_pendingXHRReplayData;

    ThreadableLoaderClient* m_pendingEventSource;
    typedef HashMap<ThreadableLoaderClient*, unsigned long> EventSourceRequestIdMap;
    EventSourceRequestIdMap m_eventSourceRequestIdMap;

    typedef HashMap<String, RefPtr<TypeBuilder::Network::Initiator> > FrameNavigationInitiatorMap;
    FrameNavigationInitiatorMap m_frameNavigationInitiatorMap;

    // FIXME: InspectorResourceAgent should now be aware of style recalculation.
    RefPtr<TypeBuilder::Network::Initiator> m_styleRecalculationInitiator;
    bool m_isRecalculatingStyle;

    WillBeHeapHashSet<RefPtrWillBeMember<XMLHttpRequest> > m_replayXHRs;
    WillBeHeapHashSet<RefPtrWillBeMember<XMLHttpRequest> > m_replayXHRsToBeDeleted;
    Timer<InspectorResourceAgent> m_removeFinishedReplayXHRTimer;
};

} // namespace blink


#endif // !defined(InspectorResourceAgent_h)
