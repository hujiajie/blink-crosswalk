// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RecordingImageBufferSurface_h
#define RecordingImageBufferSurface_h

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBufferSurface.h"
#include "public/platform/WebThread.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "wtf/LinkedStack.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"

class SkPicture;
class SkPictureRecorder;
class RecordingImageBufferSurfaceTest;

namespace blink {

class ImageBuffer;

class RecordingImageBufferFallbackSurfaceFactory {
public:
    virtual PassOwnPtr<ImageBufferSurface> createSurface(const IntSize&, OpacityMode) = 0;
    virtual ~RecordingImageBufferFallbackSurfaceFactory() { }
};

class PLATFORM_EXPORT RecordingImageBufferSurface : public ImageBufferSurface {
    WTF_MAKE_NONCOPYABLE(RecordingImageBufferSurface); WTF_MAKE_FAST_ALLOCATED;
public:
    RecordingImageBufferSurface(const IntSize&, PassOwnPtr<RecordingImageBufferFallbackSurfaceFactory> fallbackFactory, OpacityMode = NonOpaque);
    virtual ~RecordingImageBufferSurface();

    // Implementation of ImageBufferSurface interfaces
    SkCanvas* canvas() const override;
    PassRefPtr<SkPicture> getPicture() override;
    void willDrawVideo() override;
    void didDraw() override;
    bool isValid() const override { return true; }
    bool isRecording() const override { return !m_fallbackSurface; }
    void willAccessPixels() override;
    void willOverwriteCanvas() override;
    void finalizeFrame(const FloatRect&) override;
    void setImageBuffer(ImageBuffer*) override;
    PassRefPtr<SkImage> newImageSnapshot() const override;
    bool needsClipTracking() const override { return !m_fallbackSurface; }
    void draw(GraphicsContext*, const FloatRect& destRect, const FloatRect& srcRect, SkXfermode::Mode, bool needsCopy) override;

    // Passthroughs to fallback surface
    const SkBitmap& bitmap() override;
    bool restore() override;
    WebLayer* layer() const override;
    bool isAccelerated() const override;
    Platform3DObject getBackingTexture() const override;
    bool cachedBitmapEnabled() const override;
    const SkBitmap& cachedBitmap() const override;
    void invalidateCachedBitmap() override;
    void updateCachedBitmapIfNeeded() override;
    void setIsHidden(bool) override;

private:
    friend class ::RecordingImageBufferSurfaceTest; // for unit testing
    void fallBackToRasterCanvas();
    bool initializeCurrentFrame();
    bool finalizeFrameInternal();

    OwnPtr<SkPictureRecorder> m_currentFrame;
    RefPtr<SkPicture> m_previousFrame;
    OwnPtr<ImageBufferSurface> m_fallbackSurface;
    ImageBuffer* m_imageBuffer;
    int m_initialSaveCount;
    bool m_frameWasCleared;
    bool m_didRecordDrawCommandsInCurrentFrame;
    OwnPtr<RecordingImageBufferFallbackSurfaceFactory> m_fallbackFactory;
};

} // namespace blink

#endif
