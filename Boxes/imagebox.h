#ifndef IMAGEBOX_H
#define IMAGEBOX_H
#include "Boxes/boundingbox.h"
#include "skiaincludes.h"

struct ImageBoxRenderData : public BoundingBoxRenderData {
    sk_sp<SkImage> image;
private:
    void drawSk(SkCanvas *canvas) {
        SkPaint paint;
        //paint.setFilterQuality(kHigh_SkFilterQuality);
        canvas->drawImage(image, 0, 0, &paint);
    }
};

class ImageBox : public BoundingBox {
public:
    ImageBox(QString filePath = "");

    void reloadPixmap();
    void setFilePath(QString path);

    void makeDuplicate(Property *targetBox);
    BoundingBox *createNewDuplicate();

    bool SWT_isImageBox() { return true; }
    void addActionsToMenu(QMenu *menu);
    bool handleSelectedCanvasAction(QAction *selectedAction);
    void changeSourceFile();

    void setupBoundingBoxRenderDataForRelFrame(const int &relFrame,
                                               BoundingBoxRenderData *data);

    BoundingBoxRenderData *createRenderData() {
        return new ImageBoxRenderData();
    }

private:
    sk_sp<SkImage> mImageSk;
    QString mImageFilePath;
};

#endif // IMAGEBOX_H
