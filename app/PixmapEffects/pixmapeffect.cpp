#include "pixmapeffect.h"
#include <QDrag>

PixmapEffectRenderData::~PixmapEffectRenderData() {}

PixmapEffect::PixmapEffect(const QString &name,
                           const PixmapEffectType &type) :
    ComplexAnimator(name) {
    mType = type;
}

bool PixmapEffect::interrupted() {
    if(mInterrupted) {
        mInterrupted = false;
        return true;
    }
    return false;
}

qreal PixmapEffect::getMargin() { return 0.; }

qreal PixmapEffect::getMarginAtRelFrame(const int ) { return 0.; }

void PixmapEffect::prp_startDragging() {
    QMimeData *mimeData = new PixmapEffectMimeData(this);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec();
}

QMimeData *PixmapEffect::SWT_createMimeData() {
    return new PixmapEffectMimeData(this);
}

bool PixmapEffect::SWT_isPixmapEffect() const { return true; }

void PixmapEffect::switchVisible() {
    setVisible(!mVisible);
}

void PixmapEffect::setVisible(const bool visible) {
    if(visible == mVisible) return;
    mVisible = visible;
    prp_afterWholeInfluenceRangeChanged();
}

bool PixmapEffect::isVisible() const {
    return mVisible;
}

void PixmapEffect::interrupt() {
    mInterrupted = true;
}
