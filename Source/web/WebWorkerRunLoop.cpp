/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/platform/WebWorkerRunLoop.h"

#include "core/workers/WorkerThread.h"

namespace blink {

namespace {

class TaskForwarder : public ExecutionContextTask {
public:
    static PassOwnPtr<TaskForwarder> create(PassOwnPtr<WebWorkerRunLoop::Task> task)
    {
        return adoptPtr(new TaskForwarder(task));
    }

    virtual void performTask(ExecutionContext*)
    {
        m_task->Run();
    }

private:
    TaskForwarder(PassOwnPtr<WebWorkerRunLoop::Task> task)
        : m_task(task)
    {
    }

    OwnPtr<WebWorkerRunLoop::Task> m_task;
};

}

WebWorkerRunLoop::WebWorkerRunLoop(WorkerThread* workerThread)
    : m_workerThread(workerThread)
{
}

bool WebWorkerRunLoop::postTask(Task* task)
{
    m_workerThread->postTask(TaskForwarder::create(adoptPtr(task)));
    return !m_workerThread->terminated();
}

bool WebWorkerRunLoop::equals(const WebWorkerRunLoop& o) const
{
    return m_workerThread == o.m_workerThread;
}

} // namespace blink
