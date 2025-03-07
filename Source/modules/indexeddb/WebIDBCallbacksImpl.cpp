/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. AND ITS CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GOOGLE INC.
 * OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/indexeddb/WebIDBCallbacksImpl.h"

#include "core/dom/DOMError.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "modules/indexeddb/IDBMetadata.h"
#include "modules/indexeddb/IDBRequest.h"
#include "platform/SharedBuffer.h"
#include "public/platform/WebBlobInfo.h"
#include "public/platform/WebData.h"
#include "public/platform/WebIDBCursor.h"
#include "public/platform/WebIDBDatabase.h"
#include "public/platform/WebIDBDatabaseError.h"
#include "public/platform/WebIDBKey.h"

using blink::WebBlobInfo;
using blink::WebData;
using blink::WebIDBCursor;
using blink::WebIDBDatabase;
using blink::WebIDBDatabaseError;
using blink::WebIDBKey;
using blink::WebIDBKeyPath;
using blink::WebIDBMetadata;
using blink::WebVector;

namespace blink {

// static
PassOwnPtr<WebIDBCallbacksImpl> WebIDBCallbacksImpl::create(IDBRequest* request)
{
    return adoptPtr(new WebIDBCallbacksImpl(request));
}

WebIDBCallbacksImpl::WebIDBCallbacksImpl(IDBRequest* request)
    : m_request(request)
{
    m_asyncOperationId = InspectorInstrumentation::traceAsyncOperationStarting(m_request->executionContext(), "IndexedDB");
}

WebIDBCallbacksImpl::~WebIDBCallbacksImpl()
{
    InspectorInstrumentation::traceAsyncOperationCompleted(m_request->executionContext(), m_asyncOperationId);
}

static PassOwnPtr<Vector<WebBlobInfo>> ConvertBlobInfo(const WebVector<WebBlobInfo>& webBlobInfo)
{
    OwnPtr<Vector<WebBlobInfo>> blobInfo = adoptPtr(new Vector<WebBlobInfo>(webBlobInfo.size()));
    for (size_t i = 0; i < webBlobInfo.size(); ++i)
        (*blobInfo)[i] = webBlobInfo[i];
    return blobInfo.release();
}

void WebIDBCallbacksImpl::onError(const WebIDBDatabaseError& error)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onError(error);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(const WebVector<blink::WebString>& webStringList)
{
    Vector<String> stringList;
    for (size_t i = 0; i < webStringList.size(); ++i)
        stringList.append(webStringList[i]);
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(stringList);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(WebIDBCursor* cursor, const WebIDBKey& key, const WebIDBKey& primaryKey, const WebData& value, const WebVector<WebBlobInfo>& webBlobInfo)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(adoptPtr(cursor), key, primaryKey, value, ConvertBlobInfo(webBlobInfo));
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(WebIDBDatabase* backend, const WebIDBMetadata& metadata)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(adoptPtr(backend), metadata);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(const WebIDBKey& key)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(key);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(const WebData& value, const WebVector<WebBlobInfo>& webBlobInfo)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(value, ConvertBlobInfo(webBlobInfo));
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(const WebData& value, const WebVector<WebBlobInfo>& webBlobInfo, const WebIDBKey& key, const WebIDBKeyPath& keyPath)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(value, ConvertBlobInfo(webBlobInfo), key, keyPath);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(long long value)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(value);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess()
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess();
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onSuccess(const WebIDBKey& key, const WebIDBKey& primaryKey, const WebData& value, const WebVector<WebBlobInfo>& webBlobInfo)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onSuccess(key, primaryKey, value, ConvertBlobInfo(webBlobInfo));
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onBlocked(long long oldVersion)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onBlocked(oldVersion);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

void WebIDBCallbacksImpl::onUpgradeNeeded(long long oldVersion, WebIDBDatabase* database, const WebIDBMetadata& metadata, unsigned short dataLoss, blink::WebString dataLossMessage)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(m_request->executionContext(), m_asyncOperationId);
    m_request->onUpgradeNeeded(oldVersion, adoptPtr(database), metadata, static_cast<blink::WebIDBDataLoss>(dataLoss), dataLossMessage);
    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

} // namespace blink
