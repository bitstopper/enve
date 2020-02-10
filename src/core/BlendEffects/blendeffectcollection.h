#ifndef BLENDEFFECTCOLLECTION_H
#define BLENDEFFECTCOLLECTION_H

#include "Animators/dynamiccomplexanimator.h"

#include "blendeffect.h"

qsptr<BlendEffect> readIdCreateBlendEffect(eReadStream& src);
void writeBlendEffectType(BlendEffect* const obj, eWriteStream& dst);

typedef DynamicComplexAnimator<
    BlendEffect,
    &writeBlendEffectType,
    &readIdCreateBlendEffect> BlendEffectCollectionBase;

class BlendEffectCollection : public BlendEffectCollectionBase {
    Q_OBJECT
    e_OBJECT
protected:
    BlendEffectCollection();
public:
    void prp_setupTreeViewMenu(PropertyMenu * const menu);

    void prp_writeProperty(eWriteStream &dst) const;
    void prp_readProperty(eReadStream &src);

    void blendSetup(ChildRenderData &data,
                    const int index, const qreal relFrame,
                    QList<ChildRenderData> &delayed) const;
    void drawBlendSetup(BoundingBox * const boxToDraw,
                        SkCanvas * const canvas,
                        const SkFilterQuality filter, int& drawId,
                        QList<BlendEffect::Delayed> &delayed) const;

};

#endif // BLENDEFFECTCOLLECTION_H