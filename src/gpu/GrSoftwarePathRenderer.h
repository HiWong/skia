/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrSoftwarePathRenderer_DEFINED
#define GrSoftwarePathRenderer_DEFINED

#include "GrPathRenderer.h"

class GrTextureProvider;

/**
 * This class uses the software side to render a path to an SkBitmap and
 * then uploads the result to the gpu
 */
class GrSoftwarePathRenderer : public GrPathRenderer {
public:
    GrSoftwarePathRenderer(GrTextureProvider* texProvider, bool allowCaching)
            : fTexProvider(texProvider)
            , fAllowCaching(allowCaching) {}
private:
    static void DrawNonAARect(GrRenderTargetContext* renderTargetContext,
                              const GrPaint& paint,
                              const GrUserStencilSettings& userStencilSettings,
                              const GrClip& clip,
                              const SkMatrix& viewMatrix,
                              const SkRect& rect,
                              const SkMatrix& localMatrix);
    static void DrawAroundInvPath(GrRenderTargetContext* renderTargetContext,
                                  const GrPaint& paint,
                                  const GrUserStencilSettings& userStencilSettings,
                                  const GrClip& clip,
                                  const SkMatrix& viewMatrix,
                                  const SkIRect& devClipBounds,
                                  const SkIRect& devPathBounds);

    StencilSupport onGetStencilSupport(const GrShape&) const override {
        return GrPathRenderer::kNoSupport_StencilSupport;
    }

    bool onCanDrawPath(const CanDrawPathArgs&) const override;

    bool onDrawPath(const DrawPathArgs&) override;

private:
    GrTextureProvider*     fTexProvider;
    bool                   fAllowCaching;

    typedef GrPathRenderer INHERITED;
};

#endif
