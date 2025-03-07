/*
 * Copyright (c) 2011 Google Inc. All rights reserved.
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
#include "bindings/core/v8/WorkerScriptDebugServer.h"

#include "bindings/core/v8/V8ScriptRunner.h"
#include "core/inspector/ScriptDebugListener.h"
#include "core/inspector/WorkerDebuggerAgent.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerThread.h"
#include "wtf/MessageQueue.h"
#include <v8.h>

namespace blink {

WorkerScriptDebugServer::WorkerScriptDebugServer(WorkerGlobalScope* workerGlobalScope)
    : ScriptDebugServer(v8::Isolate::GetCurrent())
    , m_listener(0)
    , m_workerGlobalScope(workerGlobalScope)
{
    ASSERT(m_isolate);
}

void WorkerScriptDebugServer::trace(Visitor* visitor)
{
    visitor->trace(m_workerGlobalScope);
    ScriptDebugServer::trace(visitor);
}

void WorkerScriptDebugServer::addListener(ScriptDebugListener* listener)
{
    v8::HandleScope scope(m_isolate);
    ASSERT(!m_listener);

    v8::Debug::SetDebugEventListener(&WorkerScriptDebugServer::v8DebugEventCallback, v8::External::New(m_isolate, this));
    ensureDebuggerScriptCompiled();
    m_listener = listener;

    v8::Local<v8::Context> debuggerContext = v8::Debug::GetDebugContext();
    v8::Context::Scope contextScope(debuggerContext);

    v8::Local<v8::Object> debuggerScript = m_debuggerScript.newLocal(m_isolate);
    ASSERT(!debuggerScript->IsUndefined());

    v8::Handle<v8::Function> getScriptsFunction = v8::Local<v8::Function>::Cast(debuggerScript->Get(v8AtomicString(m_isolate, "getWorkerScripts")));
    v8::Handle<v8::Value> value = V8ScriptRunner::callInternalFunction(getScriptsFunction, debuggerScript, 0, 0, m_isolate);
    if (value.IsEmpty())
        return;
    ASSERT(!value->IsUndefined() && value->IsArray());
    v8::Handle<v8::Array> scriptsArray = v8::Handle<v8::Array>::Cast(value);
    for (unsigned i = 0; i < scriptsArray->Length(); ++i)
        dispatchDidParseSource(listener, v8::Handle<v8::Object>::Cast(scriptsArray->Get(v8::Integer::New(m_isolate, i))), CompileSuccess);
}

void WorkerScriptDebugServer::removeListener(ScriptDebugListener* listener)
{
    ASSERT(m_listener == listener);
    continueProgram();
    discardDebuggerScript();
    m_listener = 0;
    v8::Debug::SetDebugEventListener(0);
}

void WorkerScriptDebugServer::interruptAndRunTask(PassOwnPtr<Task> task)
{
    interruptAndRun(m_isolate, task);
}

ScriptDebugListener* WorkerScriptDebugServer::getDebugListenerForContext(v8::Handle<v8::Context>)
{
    // There is only one worker context in isolate.
    return m_listener;
}

void WorkerScriptDebugServer::runMessageLoopOnPause(v8::Handle<v8::Context>)
{
    MessageQueueWaitResult result;
    m_workerGlobalScope->thread()->willEnterNestedLoop();
    do {
        result = m_workerGlobalScope->thread()->runDebuggerTask();
    // Keep waiting until execution is resumed.
    } while (result == MessageQueueMessageReceived && isPaused());
    m_workerGlobalScope->thread()->didLeaveNestedLoop();

    // The listener may have been removed in the nested loop.
    if (m_listener)
        m_listener->didContinue();
}

void WorkerScriptDebugServer::quitMessageLoopOnPause()
{
    // Nothing to do here in case of workers since runMessageLoopOnPause will check for paused state after each debugger command.
}

} // namespace blink
