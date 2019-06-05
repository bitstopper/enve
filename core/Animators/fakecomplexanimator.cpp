#include "fakecomplexanimator.h"
#include <QPainter>

FakeComplexAnimator::FakeComplexAnimator(const QString &name, Property *target) :
    ComplexAnimator(name) {
    mTarget = target;
}

Property *FakeComplexAnimator::getTarget() {
    return mTarget;
}

void FakeComplexAnimator::drawTimelineControls(QPainter * const p,
                                               const qreal pixelsPerFrame,
                                               const FrameRange &absFrameRange,
                                               const int rowHeight) {
    if(mTarget->SWT_isAnimator()) {
        const auto aTarget = GetAsPtr(mTarget, Animator);
        aTarget->drawTimelineControls(p, pixelsPerFrame,
                                      absFrameRange, rowHeight);
    }
    ComplexAnimator::drawTimelineControls(p, pixelsPerFrame,
                                          absFrameRange, rowHeight);
}

Key *FakeComplexAnimator::anim_getKeyAtPos(const qreal relX,
                                          const int minViewedFrame,
                                          const qreal pixelsPerFrame,
                                          const int keyRectSize) {
    Key *key = ComplexAnimator::anim_getKeyAtPos(relX, minViewedFrame,
                                                pixelsPerFrame,
                                                keyRectSize);
    if(key) return key;
    if(!mTarget->SWT_isAnimator()) return nullptr;
    const auto aTarget = GetAsPtr(mTarget, Animator);
    return aTarget->anim_getKeyAtPos(relX, minViewedFrame,
                                     pixelsPerFrame, keyRectSize);
}

void FakeComplexAnimator::anim_getKeysInRect(const QRectF &selectionRect,
                                            const qreal pixelsPerFrame,
                                            QList<Key *> &keysList,
                                            const int keyRectSize) {
    if(mTarget->SWT_isAnimator()) {
        const auto aTarget = GetAsPtr(mTarget, Animator);
        aTarget->anim_getKeysInRect(selectionRect, pixelsPerFrame,
                                   keysList, keyRectSize);
    }
    ComplexAnimator::anim_getKeysInRect(selectionRect, pixelsPerFrame,
                                       keysList, keyRectSize);
}

bool FakeComplexAnimator::SWT_isFakeComplexAnimator() const { return true; }
