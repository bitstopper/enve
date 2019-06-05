#ifndef CONTRASTEFFECT_H
#define CONTRASTEFFECT_H
#include "pixmapeffect.h"

struct ContrastEffectRenderData : public PixmapEffectRenderData {
    friend class StdSelfRef;

    void applyEffectsSk(const SkBitmap &bitmap,
                        const qreal scale);

    bool hasKeys = false;
    qreal contrast;
private:
    ContrastEffectRenderData() {}
};

class ContrastEffect : public PixmapEffect {
    friend class SelfRef;
public:
    qreal getMargin() { return 0.; }

    stdsptr<PixmapEffectRenderData> getPixmapEffectRenderDataForRelFrameF(
            const qreal relFrame, BoundingBoxRenderData*);
    void writeProperty(QIODevice * const target) const;
    void readProperty(QIODevice *target);
protected:
    ContrastEffect(qreal contrast = .0);
private:
    qsptr<QrealAnimator> mContrastAnimator;
};

#endif // CONTRASTEFFECT_H
