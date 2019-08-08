#ifndef CANVAS_H
#define CANVAS_H

#include "Boxes/containerbox.h"
#include "colorhelpers.h"
#include <QThread>
#include "CacheHandlers/hddcachablecachehandler.h"
#include "skia/skiaincludes.h"
#include "GUI/valueinput.h"
#include "Animators/coloranimator.h"
#include "MovablePoints/segment.h"
#include "MovablePoints/movablepoint.h"
#include "Boxes/canvasrenderdata.h"
#include "Paint/drawableautotiledsurface.h"
#include "canvasbase.h"
#include "Paint/animatedsurface.h"
#include <QAction>
#include "Animators/outlinesettingsanimator.h"
#include "document.h"
#include "Paint/painttarget.h"

class AnimatedSurface;
class PaintBox;
class TextBox;
class Circle;
class ParticleBox;
class Rectangle;
class PathPivot;
class SoundComposition;
class SkCanvas;
class ImageSequenceBox;
class Brush;
class UndoRedoStack;
class ExternalLinkBox;
struct ShaderEffectCreator;
class SingleSound;
class VideoBox;
class ImageBox;

enum CtrlsMode : short;

class MouseEvent {
protected:
    MouseEvent(const QPointF& pos,
               const QPointF& lastPos,
               const QPointF& lastPressPos,
               const bool mouseGrabbing,
               const qreal scale,
               const QPoint& globalPos,
               const Qt::MouseButton button,
               const Qt::MouseButtons buttons,
               const Qt::KeyboardModifiers modifiers,
               const ulong& timestamp,
               std::function<void()> releaseMouse,
               std::function<void()> grabMouse,
               QWidget * const widget) :
        fPos(pos), fLastPos(lastPos), fLastPressPos(lastPressPos),
        fMouseGrabbing(mouseGrabbing), fScale(scale),
        fGlobalPos(globalPos), fButton(button), fButtons(buttons),
        fModifiers(modifiers), fTimestamp(timestamp),
        fReleaseMouse(releaseMouse), fGrabMouse(grabMouse),
        fWidget(widget) {}
public:
    MouseEvent(const QPointF& pos,
               const QPointF& lastPos,
               const QPointF& lastPressPos,
               const bool mouseGrabbing,
               const qreal scale,
               const QMouseEvent * const e,
               std::function<void()> releaseMouse,
               std::function<void()> grabMouse,
               QWidget * const widget) :
        MouseEvent(pos, lastPos, lastPressPos, mouseGrabbing,
                   scale, e->globalPos(), e->button(),
                   e->buttons(), e->modifiers(), e->timestamp(),
                   releaseMouse, grabMouse, widget) {}

    bool shiftMod() const {
        return fModifiers & Qt::SHIFT;
    }

    bool ctrlMod() const {
        return fModifiers & Qt::CTRL;
    }

    QPointF fPos;
    QPointF fLastPos;
    QPointF fLastPressPos;
    bool fMouseGrabbing;
    qreal fScale;
    QPoint fGlobalPos;
    Qt::MouseButton fButton;
    Qt::MouseButtons fButtons;
    Qt::KeyboardModifiers fModifiers;
    ulong fTimestamp;
    std::function<void()> fReleaseMouse;
    std::function<void()> fGrabMouse;
    QWidget* fWidget;
};

struct KeyEvent : public MouseEvent {
    KeyEvent(const QPointF& pos,
             const QPointF& lastPos,
             const QPointF& lastPressPos,
             const bool mouseGrabbing,
             const qreal scale,
             const QPoint globalPos,
             const Qt::MouseButtons buttons,
             const QKeyEvent * const e,
             std::function<void()> releaseMouse,
             std::function<void()> grabMouse,
             QWidget * const widget) :
      MouseEvent(pos, lastPos, lastPressPos, mouseGrabbing,
                 scale, globalPos, Qt::NoButton,
                 buttons, e->modifiers(), e->timestamp(),
                 releaseMouse, grabMouse, widget),
                 fType(e->type()), fKey(e->key()) {}

    QEvent::Type fType;
    int fKey;
};

class Canvas : public ContainerBox, public CanvasBase {
    friend class CanvasWindow;
    Q_OBJECT
    e_OBJECT
public:
    explicit Canvas(Document& document,
                    const int canvasWidth = 1920,
                    const int canvasHeight = 1080,
                    const int frameCount = 200,
                    const qreal fps = 24);
public:
    void prp_afterChangedAbsRange(const FrameRange &range);

    void selectOnlyLastPressedBox();
    void selectOnlyLastPressedPoint();

    void repaintIfNeeded();
    void setCanvasMode(const CanvasMode mode);
    void startSelectionAtPoint(const QPointF &pos);
    void moveSecondSelectionPoint(const QPointF &pos);
    void setPointCtrlsMode(const CtrlsMode mode);
    void setCurrentBoxesGroup(ContainerBox * const group);

    void updatePivot();

    void updatePivotIfNeeded();

    //void updateAfterFrameChanged(const int currentFrame);

    QSize getCanvasSize();

    void centerPivotPosition() {}

    //
    void finishSelectedPointsTransform();
    void finishSelectedBoxesTransform();
    void moveSelectedPointsByAbs(const QPointF &by,
                                 const bool startTransform);
    void moveSelectedBoxesByAbs(const QPointF &by,
                                const bool startTransform);
    void groupSelectedBoxes();

    //void selectAllBoxes();
    void deselectAllBoxes();

    void applyShadowToSelected();

    void selectedPathsUnion();
    void selectedPathsDifference();
    void selectedPathsIntersection();
    void selectedPathsDivision();
    void selectedPathsExclusion();
    void makeSelectedPointsSegmentsCurves();
    void makeSelectedPointsSegmentsLines();

    void centerPivotForSelected();
    void resetSelectedScale();
    void resetSelectedTranslation();
    void resetSelectedRotation();
    void convertSelectedBoxesToPath();
    void convertSelectedPathStrokesToPath();

    void applySampledMotionBlurToSelected();
    void applyLinesEffectToSelected();
    void applyCirclesEffectToSelected();
    void applySwirlEffectToSelected();
    void applyOilEffectToSelected();
    void applyImplodeEffectToSelected();
    void applyDesaturateEffectToSelected();
    void applyColorizeEffectToSelected();
    void applyReplaceColorEffectToSelected();
    void applyContrastEffectToSelected();
    void applyBrightnessEffectToSelected();

    void rotateSelectedBy(const qreal rotBy,
                          const QPointF &absOrigin,
                          const bool startTrans);

    QPointF getSelectedBoxesAbsPivotPos();
    bool isBoxSelectionEmpty() const;

    void ungroupSelectedBoxes();
    void scaleSelectedBy(const qreal scaleBy,
                         const QPointF &absOrigin,
                         const bool startTrans);
    void cancelSelectedBoxesTransform();
    void cancelSelectedPointsTransform();

    void setSelectedCapStyle(const SkPaint::Cap capStyle);
    void setSelectedJoinStyle(const SkPaint::Join joinStyle);

    void strokeWidthAction(const QrealAction &action);

    void startSelectedStrokeColorTransform();
    void startSelectedFillColorTransform();

    void getDisplayedFillStrokeSettingsFromLastSelected(
            PaintSettingsAnimator*& fillSetings, OutlineSettingsAnimator*& strokeSettings);
    void scaleSelectedBy(const qreal scaleXBy, const qreal scaleYBy,
                         const QPointF &absOrigin, const bool startTrans);

    qreal getResolutionFraction();
    void setResolutionFraction(const qreal percent);

    void applyCurrentTransformationToSelected();
    QPointF getSelectedPointsAbsPivotPos();
    bool isPointSelectionEmpty() const;
    void scaleSelectedPointsBy(const qreal scaleXBy,
                               const qreal scaleYBy,
                               const QPointF &absOrigin,
                               const bool startTrans);
    void rotateSelectedPointsBy(const qreal rotBy,
                                const QPointF &absOrigin,
                                const bool startTrans);
    int getPointsSelectionCount() const ;

    void clearPointsSelectionOrDeselect();
    NormalSegment getSegment(const MouseEvent &e) const;

    void createLinkBoxForSelected();
    void startSelectedPointsTransform();

    void mergePoints();
    void disconnectPoints();
    void connectPoints();

    void setSelectedFontFamilyAndStyle(const QString& family,
                                       const QString& style);
    void setSelectedFontSize(const qreal size);
    void removeSelectedPointsAndClearList();
    void removeSelectedBoxesAndClearList();

    BoundingBox* getCurrentBox() const { return mCurrentBox; }
    void setCurrentBox(BoundingBox* const box);
    void addBoxToSelection(BoundingBox * const box);
    void removeBoxFromSelection(BoundingBox * const box);
    void clearBoxesSelection();
    void clearBoxesSelectionList();

    void addPointToSelection(MovablePoint * const point);
    void removePointFromSelection(MovablePoint * const point);

    void clearPointsSelection();
    void raiseSelectedBoxesToTop();
    void lowerSelectedBoxesToBottom();
    void raiseSelectedBoxes();
    void lowerSelectedBoxes();

    void selectAndAddContainedPointsToSelection(const QRectF &absRect);
//
    void newPaintBox(const QPointF &pos);

    void mousePressEvent(const MouseEvent &e);
    void mouseReleaseEvent(const MouseEvent &e);
    void mouseMoveEvent(const MouseEvent &e);
    void mouseDoubleClickEvent(const MouseEvent &e);

    struct TabletEvent {
        TabletEvent(const QPointF& pos, QTabletEvent * const e) :
            fPos(pos), fType(e->type()),
            fButton(e->button()), fButtons(e->buttons()),
            fModifiers(e->modifiers()), fTimestamp(e->timestamp()) {}

        QPointF fPos;
        QEvent::Type fType;
        Qt::MouseButton fButton;
        Qt::MouseButtons fButtons;
        Qt::KeyboardModifiers fModifiers;
        ulong fTimestamp;
        qreal fPressure;
        int fXTilt;
        int fYTilt;
    };

    void tabletEvent(const QTabletEvent * const e, const QPointF &pos);

    bool keyPressEvent(QKeyEvent *event);

    qsptr<BoundingBox> createLink();
    ImageBox* createImageBox(const QString &path);
    ImageSequenceBox* createAnimationBoxForPaths(const QString &folderPath);
    VideoBox* createVideoForPath(const QString &path);
    ExternalLinkBox *createLinkToFileWithPath(const QString &path);
    SingleSound* createSoundForPath(const QString &path);

    void setPreviewing(const bool bT);
    void setOutputRendering(const bool bT);

    bool SWT_shouldBeVisible(const SWT_RulesCollection &rules,
                             const bool parentSatisfies,
                             const bool parentMainTarget) const;

    ContainerBox *getCurrentGroup() {
        return mCurrentContainer;
    }

    void updateTotalTransform() {}

    QMatrix getTotalTransform() const {
        return QMatrix();
    }

    QMatrix getRelativeTransformAtCurrentFrame() {
        return QMatrix();
    }

    QPointF mapAbsPosToRel(const QPointF &absPos) {
        return absPos;
    }

    void setIsCurrentCanvas(const bool bT);

    void scheduleEffectsMarginUpdate() {}

    void renderSk(SkCanvas * const canvas,
                  const QRect &drawRect,
                  const QMatrix &viewTrans,
                  const bool mouseGrabbing);

    void setCanvasSize(const int width, const int height) {
        mWidth = width;
        mHeight = height;
    }

    int getCanvasWidth() const {
        return mWidth;
    }

    QRect getCanvasBounds() const {
        return QRect(0, 0, mWidth, mHeight);
    }

    QRect getMaxBounds() const {
        return QRect(-mWidth/2, - mHeight/2, 2*mWidth, 2*mHeight);
    }

    QRect getCurrentBounds() const {
        if(mClipToCanvasSize) return getCanvasBounds();
        else return getMaxBounds();
    }

    int getCanvasHeight() const {
        return mHeight;
    }

    void setFrameRange(const FrameRange& range);

    ColorAnimator *getBgColorAnimator() {
        return mBackgroundColor.get();
    }

    stdsptr<BoxRenderData> createRenderData();

    void setupRenderData(const qreal relFrame,
                         BoxRenderData * const data) {
        ContainerBox::setupRenderData(relFrame, data);
        auto canvasData = static_cast<CanvasRenderData*>(data);
        canvasData->fBgColor = toSkColor(mBackgroundColor->getColor());
        canvasData->fCanvasHeight = mHeight;
        canvasData->fCanvasWidth = mWidth;
    }

    bool clipToCanvas() { return mClipToCanvasSize; }

    void schedulePivotUpdate();
    void setClipToCanvas(const bool bT) { mClipToCanvasSize = bT; }
    void setRasterEffectsVisible(const bool bT) { mRasterEffectsVisible = bT; }
    void setPathEffectsVisible(const bool bT) { mPathEffectsVisible = bT; }
protected:
    void setCurrentSmartEndPoint(SmartNodePoint * const point);

    void handleMovePathMouseRelease(const MouseEvent &e);
    void handleMovePointMouseRelease(const MouseEvent &e);

    void handleRightButtonMousePress(const MouseEvent &e);
    void handleLeftButtonMousePress(const MouseEvent &e);
signals:
    void requestCanvasMode(CanvasMode);
    void requestUpdate();
    void newFrameRange(FrameRange);
    void currentBoxChanged(BoundingBox*);
    void selectedPaintSettingsChanged();
    void currentFrameChanged(int);
    void currentContainerSet(ContainerBox*);
public:
    void makePointCtrlsSymmetric();
    void makePointCtrlsSmooth();
    void makePointCtrlsCorner();

    void makeSegmentLine();
    void makeSegmentCurve();

    MovablePoint *getPointAtAbsPos(const QPointF &absPos,
                                   const CanvasMode mode,
                                   const qreal invScale);
    void duplicateSelectedBoxes();
    void clearLastPressedPoint();
    void clearCurrentSmartEndPoint();
    void applyPaintSettingToSelected(const PaintSettingsApplier &setting);

    int getCurrentFrame();
    FrameRange getFrameRange() const { return mRange; }

    SoundComposition *getSoundComposition();

    void updateHoveredBox(const MouseEvent& e);
    void updateHoveredPoint(const MouseEvent& e);
    void updateHoveredEdge(const MouseEvent &e);
    void updateHovered(const MouseEvent &e);
    void clearHoveredEdge();
    void clearHovered();

    bool getPivotLocal() const {
        return mDocument.fLocalPivot;
    }

    int getMaxFrame();

    //void updatePixmaps();
    HDDCachableCacheHandler& getCacheHandler() {
        return mCacheHandler;
    }

    HDDCachableCacheHandler& getSoundCacheHandler();

    void setCurrentPreviewContainer(const int relFrame);
    void setCurrentPreviewContainer(const stdsptr<ImageCacheContainer> &cont);
    void setLoadingPreviewContainer(
            const stdsptr<ImageCacheContainer> &cont);

    void setRenderingPreview(const bool bT);

    bool isPreviewingOrRendering() const {
        return mPreviewing || mRenderingPreview || mRenderingOutput;
    }

    qreal getFps() const { return mFps; }
    void setFps(const qreal fps) { mFps = fps; }

    BoundingBox *getBoxAt(const QPointF &absPos) {
        if(mClipToCanvasSize) {
            const auto bRect = Canvas::getCurrentBounds();
            if(!QRectF(bRect).contains(absPos)) return nullptr;
        }
        return ContainerBox::getBoxAt(absPos);
    }

    void anim_scaleTime(const int pivotAbsFrame, const qreal scale);

    void changeFpsTo(const qreal fps) {
        anim_scaleTime(0, fps/mFps);
        setFps(fps);
    }

    bool SWT_isCanvas() const { return true; }

    void addActionsToMenu(QMenu* const menu);

    void deleteAction();
    void copyAction();
    void pasteAction();
    void cutAction();
    void duplicateAction();
    void selectAllAction();
    void clearSelectionAction();
    void rotateSelectedBoxesStartAndFinish(const qreal rotBy);
    bool shouldScheduleUpdate() {
        return mCurrentPreviewContainerOutdated;
    }

    void renderDataFinished(BoxRenderData *renderData);
    FrameRange prp_getIdenticalRelRange(const int relFrame) const;

    void writeBoundingBox(QIODevice * const target);
    void readBoundingBox(QIODevice * const target);
    bool anim_prevRelFrameWithKey(const int relFrame, int &prevRelFrame);
    bool anim_nextRelFrameWithKey(const int relFrame, int &nextRelFrame);

    void shiftAllPointsForAllKeys(const int by);
    void revertAllPointsForAllKeys();
    void shiftAllPoints(const int by);
    void revertAllPoints();
    void flipSelectedBoxesHorizontally();
    void flipSelectedBoxesVertically();
    int getByteCountPerFrame() {
        return qCeil(mWidth*mResolutionFraction)*
                qCeil(mHeight*mResolutionFraction)*4;
        //return mCurrentPreviewContainer->getByteCount();
    }
    int getMaxPreviewFrame(const int minFrame, const int maxFrame);
    void selectedPathsCombine();
    void selectedPathsBreakApart();
    void invertSelectionAction();

    bool getRasterEffectsVisible() const {
        return mRasterEffectsVisible;
    }

    bool getPathEffectsVisible() const {
        return mPathEffectsVisible;
    }

    void anim_setAbsFrame(const int frame);

    void moveDurationRectForAllSelected(const int dFrame);
    void startDurationRectPosTransformForAllSelected();
    void finishDurationRectPosTransformForAllSelected();
    void startMinFramePosTransformForAllSelected();
    void finishMinFramePosTransformForAllSelected();
    void moveMinFrameForAllSelected(const int dFrame);
    void startMaxFramePosTransformForAllSelected();
    void finishMaxFramePosTransformForAllSelected();
    void moveMaxFrameForAllSelected(const int dFrame);

    bool newUndoRedoSet();

    void undo();
    void redo();

    void blockUndoRedo();
    void unblockUndoRedo();

    void setParentToLastSelected();
    void clearParentForSelected();

    bool startRotatingAction(const KeyEvent &e);
    bool startScalingAction(const KeyEvent &e);
    bool startMovingAction(const KeyEvent &e);

    void deselectAllBoxesAction();
    void selectAllBoxesAction();
    void selectAllPointsAction();
    bool handlePaintModeKeyPress(const KeyEvent &e);
    bool handleModifierChange(const KeyEvent &e);
    bool handleTransormationInputKeyEvent(const KeyEvent &e);

    void setCurrentGroupParentAsCurrentGroup();

    void setCurrentRenderRange(const FrameRange& range) {
        mCurrRenderRange = range;
    }

    bool hasValidPaintTarget() const {
        return mPaintTarget.isValid();
    }

    void queScheduledTasks() {
        if(Actions::sInstance->smoothChange() && mCurrentContainer) {
            if(!mDrawnSinceQue) return;
            mCurrentContainer->queChildScheduledTasks();
        } else ContainerBox::queScheduledTasks();
        mDrawnSinceQue = false;
    }
private:
    void openTextEditorForTextBox(TextBox *textBox);

    void scaleSelected(const MouseEvent &e);
    void rotateSelected(const MouseEvent &e);
    qreal mLastDRot = 0;
    int mRotHalfCycles = 0;
    TransformMode mTransMode = MODE_NONE;
protected:
    Document& mDocument;
    bool mSmoothChange = false;
    bool mDrawnSinceQue = true;

    stdsptr<UndoRedoStack> mUndoRedoStack;

    void updatePaintBox();

    PaintTarget mPaintTarget;
    bool mStylusDrawing = false;

    uint mLastStateId = 0;
    HDDCachableCacheHandler mCacheHandler;

    qsptr<ColorAnimator> mBackgroundColor = enve::make_shared<ColorAnimator>();

    SmartVectorPath *getPathResultingFromOperation(const SkPathOp &pathOp);

    void sortSelectedBoxesAsc();
    void sortSelectedBoxesDesc();

    qsptr<SoundComposition> mSoundComposition;

    bool mLocalPivot = false;
    bool mIsCurrentCanvas = true;
    FrameRange mRange{0, 200};

    qreal mResolutionFraction;

    qptr<BoundingBox> mCurrentBox;
    qptr<Circle> mCurrentCircle;
    qptr<Rectangle> mCurrentRectangle;
    qptr<TextBox> mCurrentTextBox;
    qptr<ParticleBox> mCurrentParticleBox;
    qptr<ContainerBox> mCurrentContainer;

    stdptr<MovablePoint> mHoveredPoint_d;
    qptr<BoundingBox> mHoveredBox;

    qptr<BoundingBox> mPressedBox;
    stdsptr<PathPivot> mRotPivot;

    stdptr<SmartNodePoint> mLastEndPoint;

    NormalSegment mHoveredNormalSegment;
    NormalSegment mCurrentNormalSegment;
    qreal mCurrentNormalSegmentT;

    ValueInput mValueInput;

    bool mPreviewing = false;
    bool mRenderingPreview = false;
    bool mRenderingOutput = false;
    FrameRange mCurrRenderRange;

    bool mCurrentPreviewContainerOutdated = false;
    stdsptr<ImageCacheContainer> mCurrentPreviewContainer;
    stdsptr<ImageCacheContainer> mLoadingPreviewContainer;

    bool mClipToCanvasSize = false;
    bool mRasterEffectsVisible = true;
    bool mPathEffectsVisible = true;

    bool mDoubleClick = false;
    int mMovesToSkip = 0;

    int mWidth;
    int mHeight;
    qreal mFps;

    bool mPivotUpdateNeeded = false;

    bool mFirstMouseMove = false;
    bool mSelecting = false;
//    bool mMoving = false;

    QRectF mSelectionRect;
    CanvasMode mCurrentMode = MOVE_BOX;

    void handleMovePointMousePressEvent(const MouseEvent& e);
    void handleMovePointMouseMove(const MouseEvent& e);

    void handleMovePathMousePressEvent(const MouseEvent &e);
    void handleMovePathMouseMove(const MouseEvent &e);

    void handleLeftMouseRelease(const MouseEvent &e);

    void handleAddSmartPointMousePress(const MouseEvent &e);
    void handleAddSmartPointMouseMove(const MouseEvent &e);
    void handleAddSmartPointMouseRelease(const MouseEvent &e);

    void updateTransformation(const KeyEvent &e);
    QPointF getMoveByValueForEvent(const MouseEvent &e);
    void cancelCurrentTransform();
};

#endif // CANVAS_H