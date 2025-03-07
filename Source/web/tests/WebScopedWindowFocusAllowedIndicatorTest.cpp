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

#include "config.h"
#include "public/web/WebScopedWindowFocusAllowedIndicator.h"

#include "core/dom/Document.h"
#include "core/page/WindowFocusAllowedIndicator.h"
#include "public/web/WebDocument.h"
#include <gtest/gtest.h>

using namespace blink;

namespace {

TEST(WebScopedWindowFocusAllowedIndicatorTest, Basic)
{
    EXPECT_FALSE(WindowFocusAllowedIndicator::windowFocusAllowed());
    {
        WebScopedWindowFocusAllowedIndicator indicator1;
        EXPECT_TRUE(WindowFocusAllowedIndicator::windowFocusAllowed());
        {
            WebScopedWindowFocusAllowedIndicator indicator2;
            EXPECT_TRUE(WindowFocusAllowedIndicator::windowFocusAllowed());
        }
        EXPECT_TRUE(WindowFocusAllowedIndicator::windowFocusAllowed());
    }
    EXPECT_FALSE(WindowFocusAllowedIndicator::windowFocusAllowed());
}

TEST(WebScopedWindowFocusAllowedIndicatorTest, WithDocument)
{
    RefPtrWillBePersistent<Document> document = Document::create();
    WebDocument webDocument(document);

    EXPECT_FALSE(document->isWindowInteractionAllowed());
    {
        WebScopedWindowFocusAllowedIndicator indicator1(&webDocument);
        EXPECT_TRUE(document->isWindowInteractionAllowed());
        {
            WebScopedWindowFocusAllowedIndicator indicator2(&webDocument);
            EXPECT_TRUE(document->isWindowInteractionAllowed());
        }
        EXPECT_TRUE(document->isWindowInteractionAllowed());
    }
    EXPECT_FALSE(document->isWindowInteractionAllowed());
}

}
