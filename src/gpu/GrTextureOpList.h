/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrTexureOpList_DEFINED
#define GrTexureOpList_DEFINED

#include "GrOpList.h"

#include "SkTArray.h"

class GrAuditTrail;
class GrBatch;
class GrGpu;
class GrTexture;
struct SkIPoint;
struct SkIRect;

class GrTextureOpList final : public GrOpList {
public:
    GrTextureOpList(GrTexture*, GrGpu*, GrAuditTrail*);

    ~GrTextureOpList() override;

    /**
     * Empties the draw buffer of any queued up draws.
     */
    void reset() override;

    void abandonGpuResources() override {}
    void freeGpuResources() override {}

    /**
     * Together these two functions flush all queued up draws to GrCommandBuffer. The return value
     * of drawBatches() indicates whether any commands were actually issued to the GPU.
     */
    void prepareBatches(GrBatchFlushState* flushState) override;
    bool drawBatches(GrBatchFlushState* flushState) override;

    /**
     * Copies a pixel rectangle from one surface to another. This call may finalize
     * reserved vertex/index data (as though a draw call was made). The src pixels
     * copied are specified by srcRect. They are copied to a rect of the same
     * size in dst with top left at dstPoint. If the src rect is clipped by the
     * src bounds then  pixel values in the dst rect corresponding to area clipped
     * by the src rect are not overwritten. This method is not guaranteed to succeed
     * depending on the type of surface, configs, etc, and the backend-specific
     * limitations.
     */
    bool copySurface(GrSurface* dst,
                     GrSurface* src,
                     const SkIRect& srcRect,
                     const SkIPoint& dstPoint);

    SkDEBUGCODE(void dump() const override;)

private:
    void recordBatch(GrBatch*);

    SkSTArray<2, sk_sp<GrBatch>, true> fRecordedBatches;
    GrGpu*                             fGpu;

    typedef GrOpList INHERITED;
};

#endif
