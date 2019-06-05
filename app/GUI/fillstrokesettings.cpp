﻿#include "fillstrokesettings.h"
#include "GUI/GradientWidgets/gradientwidget.h"
#include "mainwindow.h"
#include "undoredo.h"
#include "canvas.h"
#include "qrealanimatorvalueslider.h"
#include "GUI/ColorWidgets/colorsettingswidget.h"
#include "actionbutton.h"
#include "qdoubleslider.h"
#include "segment1deditor.h"
#include "namedcontainer.h"
#include <QDockWidget>
#include "paintsettingsapplier.h"
#include "Animators/gradient.h"

FillStrokeSettingsWidget::FillStrokeSettingsWidget(MainWindow *parent) :
    QTabWidget(parent) {
    //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mMainWindow = parent;

    mGradientWidget = new GradientWidget(this, mMainWindow);
    mStrokeSettingsWidget = new QWidget(this);
    mColorsSettingsWidget = new ColorSettingsWidget(this);

    mTargetLayout->setSpacing(0);
    mFillTargetButton = new QPushButton(
                QIcon(":/icons/properties_fill.png"),
                "Fill", this);
    mFillTargetButton->setObjectName("leftButton");
    mStrokeTargetButton = new QPushButton(
                QIcon(":/icons/properties_stroke_paint.png"),
                "Stroke", this);
    mStrokeTargetButton->setObjectName("rightButton");
    mFillAndStrokeWidget = new QWidget(this);
    mFillAndStrokeWidget->setLayout(mMainLayout);
    mMainLayout->setAlignment(Qt::AlignTop);

    mColorTypeLayout = new QHBoxLayout();
    mColorTypeLayout->setSpacing(0);
    mFillNoneButton = new QPushButton(
                QIcon(":/icons/fill_none.png"),
                "None", this);
    mFillNoneButton->setCheckable(true);
    mFillNoneButton->setObjectName("leftButton");
    connect(mFillNoneButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setNoneFill);
    mFillFlatButton = new QPushButton(
                QIcon(":/icons/fill_flat.png"),
                "Flat", this);
    mFillFlatButton->setCheckable(true);
    mFillFlatButton->setObjectName("middleButton");
    connect(mFillFlatButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFlatFill);
    mFillGradientButton = new QPushButton(
                QIcon(":/icons/fill_gradient.png"),
                "Gradient", this);
    mFillGradientButton->setCheckable(true);
    mFillGradientButton->setObjectName("rightButton");
    connect(mFillGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setGradientFill);

    mFillBrushButton = new QPushButton(
                QIcon(":/icons/fill_brush.png"),
                "Brush", this);
    mFillBrushButton->setCheckable(true);
    mFillBrushButton->setObjectName("middleButton");
    connect(mFillBrushButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setBrushFill);

    mColorTypeLayout->addWidget(mFillNoneButton);
    mColorTypeLayout->addWidget(mFillFlatButton);
    mColorTypeLayout->addWidget(mFillBrushButton);
    mColorTypeLayout->addWidget(mFillGradientButton);

    mFillTargetButton->setCheckable(true);
    mFillTargetButton->setFocusPolicy(Qt::NoFocus);
    mStrokeTargetButton->setCheckable(true);
    mStrokeTargetButton->setFocusPolicy(Qt::NoFocus);
    mTargetLayout->addWidget(mFillTargetButton);
    mTargetLayout->addWidget(mStrokeTargetButton);

    //mLineWidthSpin = new QDoubleSpinBox(this);
    mLineWidthSpin = new QrealAnimatorValueSlider("line width",
                                                  0., 1000., 1., this);
    mLineWidthSpin->setNameVisible(false);
    //mLineWidthSpin->setValueSliderVisibile(false);
    //mLineWidthSpin->setRange(0.0, 1000.0);
    //mLineWidthSpin->setSuffix(" px");
    //mLineWidthSpin->setSingleStep(0.1);
    mLineWidthLayout->addWidget(mLineWidthLabel);
    mLineWidthLayout->addWidget(mLineWidthSpin, Qt::AlignLeft);

    mStrokeSettingsLayout->addLayout(mLineWidthLayout);

    mJoinStyleLayout->setSpacing(0);
    mBevelJoinStyleButton = new QPushButton(QIcon(":/icons/join_bevel.png"),
                                            "", this);
    mBevelJoinStyleButton->setObjectName("leftButton");
    mMiterJointStyleButton = new QPushButton(QIcon(":/icons/join_miter.png"),
                                             "", this);
    mMiterJointStyleButton->setObjectName("middleButton");
    mRoundJoinStyleButton = new QPushButton(QIcon(":/icons/join_round.png"),
                                            "", this);
    mRoundJoinStyleButton->setObjectName("rightButton");

    mBevelJoinStyleButton->setCheckable(true);
    mMiterJointStyleButton->setCheckable(true);
    mRoundJoinStyleButton->setCheckable(true);
    connect(mBevelJoinStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setBevelJoinStyle);
    connect(mMiterJointStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setMiterJoinStyle);
    connect(mRoundJoinStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRoundJoinStyle);

    mJoinStyleLayout->addWidget(new QLabel("Join:", this));
    mJoinStyleLayout->addWidget(mBevelJoinStyleButton);
    mJoinStyleLayout->addWidget(mMiterJointStyleButton);
    mJoinStyleLayout->addWidget(mRoundJoinStyleButton);

    mStrokeJoinCapWidget = new QWidget(this);
    const auto strokeJoinCapLay = new QVBoxLayout(mStrokeJoinCapWidget);
    mStrokeJoinCapWidget->setLayout(strokeJoinCapLay);
    strokeJoinCapLay->addLayout(mJoinStyleLayout);

    mCapStyleLayout->setSpacing(0);
    mFlatCapStyleButton = new QPushButton(QIcon(":/icons/cap_flat.png"),
                                          "", this);
    mFlatCapStyleButton->setObjectName("leftButton");
    mSquareCapStyleButton = new QPushButton(QIcon(":/icons/cap_square.png"),
                                            "", this);
    mSquareCapStyleButton->setObjectName("middleButton");
    mRoundCapStyleButton = new QPushButton(QIcon(":/icons/cap_round.png"),
                                           "", this);
    mRoundCapStyleButton->setObjectName("rightButton");
    mFlatCapStyleButton->setCheckable(true);
    mSquareCapStyleButton->setCheckable(true);
    mRoundCapStyleButton->setCheckable(true);
    connect(mFlatCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFlatCapStyle);
    connect(mSquareCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setSquareCapStyle);
    connect(mRoundCapStyleButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRoundCapStyle);

    mCapStyleLayout->addWidget(new QLabel("Cap:", this));
    mCapStyleLayout->addWidget(mFlatCapStyleButton);
    mCapStyleLayout->addWidget(mSquareCapStyleButton);
    mCapStyleLayout->addWidget(mRoundCapStyleButton);

    strokeJoinCapLay->addLayout(mCapStyleLayout);

    connect(mLineWidthSpin, &QrealAnimatorValueSlider::valueChanged,
            this, &FillStrokeSettingsWidget::setStrokeWidth);

    connect(mLineWidthSpin, &QrealAnimatorValueSlider::editingFinished,
            this, &FillStrokeSettingsWidget::emitStrokeWidthChanged);

    mStrokeSettingsLayout->addWidget(mStrokeJoinCapWidget);
    mStrokeSettingsWidget->setLayout(mStrokeSettingsLayout);

    connect(mFillTargetButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setFillTarget);
    connect(mStrokeTargetButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setStrokeTarget);

    connect(mColorsSettingsWidget, &ColorSettingsWidget::colorSettingSignal,
            this, &FillStrokeSettingsWidget::colorSettingReceived);

    connect(mColorsSettingsWidget, &ColorSettingsWidget::colorModeChanged,
            this, &FillStrokeSettingsWidget::setCurrentColorMode);

    mGradientTypeLayout = new QHBoxLayout();
    mGradientTypeLayout->setSpacing(0);
    mLinearGradientButton = new QPushButton(
                QIcon(":/icons/fill_gradient.png"),
                "Linear", this);
    mLinearGradientButton->setCheckable(true);
    mLinearGradientButton->setObjectName("leftButton");
    connect(mLinearGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setLinearGradientFill);

    mRadialGradientButton = new QPushButton(
                QIcon(":/icons/fill_gradient_radial.png"),
                "Radial", this);
    mRadialGradientButton->setCheckable(true);
    mRadialGradientButton->setObjectName("rightButton");
    connect(mRadialGradientButton, &QPushButton::released,
            this, &FillStrokeSettingsWidget::setRadialGradientFill);

    mGradientTypeLayout->addWidget(mLinearGradientButton);
    mGradientTypeLayout->addWidget(mRadialGradientButton);
    mGradientTypeWidget = new QWidget(this);
    mGradientTypeWidget->setContentsMargins(0, 0, 0, 0);
    mGradientTypeWidget->setLayout(mGradientTypeLayout);

    const auto brushCurvesWidget = new QWidget(this);
    mBrushWidthCurveEditor = new Segment1DEditor(0, 1, this);
    mBrushPressureCurveEditor = new Segment1DEditor(0, 1, this);
    mBrushSpacingCurveEditor = new Segment1DEditor(0, 50, this);
    mBrushTimeCurveEditor = new Segment1DEditor(0, 2, this);
    const auto brushCurvesLayout = new QVBoxLayout;
    brushCurvesWidget->setLayout(brushCurvesLayout);
    brushCurvesLayout->addWidget(
                new NamedContainer("width", mBrushWidthCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("pressure", mBrushPressureCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("spacing", mBrushSpacingCurveEditor, true, this));
    brushCurvesLayout->addWidget(
                new NamedContainer("time", mBrushTimeCurveEditor, true, this));
    const auto brushCurvesScroll = new ScrollArea(this);
    brushCurvesScroll->setWidget(brushCurvesWidget);
    mBrushSettingsWidget = brushCurvesScroll;

    const int ctxt = BrushSelectionWidget::sCreateNewContext();
    mBrushSelectionWidget = new BrushSelectionWidget(ctxt, this);

    connect(mBrushSelectionWidget,
            &BrushSelectionWidget::currentBrushChanged,
            this, &FillStrokeSettingsWidget::setStrokeBrush);
    connect(mBrushWidthCurveEditor, &Segment1DEditor::segmentEdited,
            this, &FillStrokeSettingsWidget::setBrushWidthCurve);
    connect(mBrushTimeCurveEditor, &Segment1DEditor::segmentEdited,
            this, &FillStrokeSettingsWidget::setBrushTimeCurve);
    connect(mBrushPressureCurveEditor, &Segment1DEditor::segmentEdited,
            this, &FillStrokeSettingsWidget::setBrushPressureCurve);
    connect(mBrushSpacingCurveEditor, &Segment1DEditor::segmentEdited,
            this, &FillStrokeSettingsWidget::setBrushSpacingCurve);

    mMainLayout->addLayout(mTargetLayout);
    mMainLayout->addLayout(mColorTypeLayout);
    mMainLayout->addWidget(mGradientTypeWidget);
    mMainLayout->addWidget(mStrokeSettingsWidget);
    mMainLayout->addWidget(mGradientWidget);
    mMainLayout->addWidget(mColorsSettingsWidget);

    setTabPosition(QTabWidget::South);
    mMainLayout->addStretch(1);

    const auto fillAndStrokeArea = new ScrollArea(this);
    fillAndStrokeArea->setWidget(mFillAndStrokeWidget);
    addTab(fillAndStrokeArea, "Fill and Stroke");
    addTab(mBrushSelectionWidget, "Stroke Brush");
    addTab(mBrushSettingsWidget, "Stroke Curves");

    mGradientTypeWidget->hide();

    setFillTarget();
    setCapStyle(Qt::RoundCap);
    setJoinStyle(Qt::RoundJoin);
}

void FillStrokeSettingsWidget::setLinearGradientFill() {
    setGradientType(Gradient::LINEAR);
}

void FillStrokeSettingsWidget::setRadialGradientFill() {
    setGradientType(Gradient::RADIAL);
}

void FillStrokeSettingsWidget::setGradientFill() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(true);
    mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(false);
    colorTypeSet(GRADIENTPAINT);
}

void FillStrokeSettingsWidget::setBrushFill() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->hide();
    mFillBrushButton->setChecked(true);
    mFillGradientButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(false);
    colorTypeSet(BRUSHPAINT);
}

void FillStrokeSettingsWidget::setFlatFill() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(false);
    mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(true);
    mFillNoneButton->setChecked(false);
    colorTypeSet(FLATPAINT);
}

void FillStrokeSettingsWidget::setNoneFill() {
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
    mFillGradientButton->setChecked(false);
    mFillBrushButton->setChecked(false);
    mFillFlatButton->setChecked(false);
    mFillNoneButton->setChecked(true);
    colorTypeSet(NOPAINT);
}

void FillStrokeSettingsWidget::updateColorAnimator() {
    if(getCurrentPaintTypeVal() == NOPAINT) {
        setColorAnimatorTarget(nullptr);
    } else if(getCurrentPaintTypeVal() == FLATPAINT ||
              getCurrentPaintTypeVal() == BRUSHPAINT) {
        if(mTarget == PaintSetting::FILL) {
            setColorAnimatorTarget(mCurrentFillColorAnimator);
        } else {
            setColorAnimatorTarget(mCurrentStrokeColorAnimator);
        }
    } else if(getCurrentPaintTypeVal() == GRADIENTPAINT) {
        setColorAnimatorTarget(mGradientWidget->getCurrentColorAnimator());
    }
}

void FillStrokeSettingsWidget::setCurrentColorMode(const ColorMode &mode) {
    if(mTarget == PaintSetting::FILL) {
        if(mCurrentFillPaintType == FLATPAINT) {
            mCanvasWindow->setSelectedFillColorMode(mode);
        }
    } else {
        if(mCurrentStrokePaintType == FLATPAINT ||
           mCurrentStrokePaintType == BRUSHPAINT) {
            mCanvasWindow->setSelectedStrokeColorMode(mode);
        }
    }
}

void FillStrokeSettingsWidget::updateAfterTargetChanged() {
    if(getCurrentPaintTypeVal() == GRADIENTPAINT) {
        mGradientWidget->setCurrentGradient(getCurrentGradientVal(), false);
        const auto gradType = getCurrentGradientTypeVal();
        mLinearGradientButton->setChecked(gradType == Gradient::LINEAR);
        mRadialGradientButton->setChecked(gradType == Gradient::RADIAL);
    }
    setCurrentPaintType(getCurrentPaintTypeVal());
    if(getCurrentPaintTypeVal() == NOPAINT) {
        mFillGradientButton->setChecked(false);
        mFillFlatButton->setChecked(false);
        mFillBrushButton->setChecked(false);
        mFillNoneButton->setChecked(true);
    } else if(getCurrentPaintTypeVal() == FLATPAINT) {
        mFillGradientButton->setChecked(false);
        mFillFlatButton->setChecked(true);
        mFillNoneButton->setChecked(false);
        mFillBrushButton->setChecked(false);
    } else if(getCurrentPaintTypeVal() == GRADIENTPAINT) {
        mFillGradientButton->setChecked(true);
        mFillFlatButton->setChecked(false);
        mFillBrushButton->setChecked(false);
        mFillNoneButton->setChecked(false);
    } else if(getCurrentPaintTypeVal() == BRUSHPAINT) {
        mFillGradientButton->setChecked(false);
        mFillFlatButton->setChecked(false);
        mFillBrushButton->setChecked(true);
        mFillNoneButton->setChecked(false);
    }
}

void FillStrokeSettingsWidget::setCurrentPaintType(
        const PaintType &paintType) {
    if(paintType == NOPAINT) setNoPaintType();
    else if(paintType == FLATPAINT) setFlatPaintType();
    else if(paintType == BRUSHPAINT) setBrushPaintType();
    else setGradientPaintType();
}

void FillStrokeSettingsWidget::setStrokeBrush(
        SimpleBrushWrapper * const brush) {
    mCurrentStrokeBrush = brush;
    emitStrokeBrushChanged();
}

void FillStrokeSettingsWidget::setBrushSpacingCurve(
        const qCubicSegment1D& seg) {
    mCurrentStrokeBrushSpacingCurve = seg;
    emitStrokeBrushSpacingCurveChanged();
}

void FillStrokeSettingsWidget::setBrushPressureCurve(
        const qCubicSegment1D& seg) {
    mCurrentStrokeBrushPressureCurve = seg;
    emitStrokeBrushPressureCurveChanged();
}

void FillStrokeSettingsWidget::setBrushWidthCurve(
        const qCubicSegment1D& seg) {
    mCurrentStrokeBrushWidthCurve = seg;
    emitStrokeBrushWidthCurveChanged();
}

void FillStrokeSettingsWidget::setBrushTimeCurve(
        const qCubicSegment1D& seg) {
    mCurrentStrokeBrushTimeCurve = seg;
    emitStrokeBrushTimeCurveChanged();
}

void FillStrokeSettingsWidget::setStrokeWidth(const qreal width) {
    //startTransform(SLOT(emitStrokeWidthChanged()));
    mCurrentStrokeWidth = width;
    emitStrokeWidthChangedTMP();
}

void FillStrokeSettingsWidget::setCurrentSettings(
        PaintSettingsAnimator *fillPaintSettings,
        OutlineSettingsAnimator *strokePaintSettings) {
    disconnect(mLineWidthSpin, &QrealAnimatorValueSlider::valueChanged,
               this, &FillStrokeSettingsWidget::setStrokeWidth);

    setFillValuesFromFillSettings(fillPaintSettings);
    setStrokeValuesFromStrokeSettings(strokePaintSettings);
    //mLineWidthSpin->setValue(strokePaintSettings->getCurrentStrokeWidth());

    if(mTarget == PaintSetting::FILL) setFillTarget();
    else setStrokeTarget();

    connect(mLineWidthSpin, &QrealAnimatorValueSlider::valueChanged,
            this, &FillStrokeSettingsWidget::setStrokeWidth);
}

GradientWidget *FillStrokeSettingsWidget::getGradientWidget() {
    return mGradientWidget;
}

void FillStrokeSettingsWidget::clearAll() {
    mGradientWidget->clearAll();
}

void FillStrokeSettingsWidget::setCurrentBrushSettings(
        BrushSettings * const brushSettings) {
    if(brushSettings) {
        BrushSelectionWidget::sSetCurrentBrushForContext(
                    mBrushSelectionWidget->getContextId(),
                    brushSettings->getBrush());
        mBrushWidthCurveEditor->setCurrentAnimator(
                    brushSettings->getWidthAnimator());
        mBrushPressureCurveEditor->setCurrentAnimator(
                    brushSettings->getPressureAnimator());
        mBrushSpacingCurveEditor->setCurrentAnimator(
                    brushSettings->getSpacingAnimator());
        mBrushTimeCurveEditor->setCurrentAnimator(
                    brushSettings->getTimeAnimator());
    } else {
        mBrushWidthCurveEditor->setCurrentAnimator(nullptr);
        mBrushPressureCurveEditor->setCurrentAnimator(nullptr);
        mBrushSpacingCurveEditor->setCurrentAnimator(nullptr);
        mBrushTimeCurveEditor->setCurrentAnimator(nullptr);
    }
}

void FillStrokeSettingsWidget::colorTypeSet(const PaintType &type) {
    if(type == NOPAINT) {
        setNoPaintType();
    } else if(type == FLATPAINT) {
        setFlatPaintType();
    } else if(type == GRADIENTPAINT) {
        if(mTarget == PaintSetting::FILL ? !mCurrentFillGradient :
                !mCurrentStrokeGradient) {
            mGradientWidget->setCurrentGradient(nullptr);
        }
        setGradientPaintType();
    } else if(type == BRUSHPAINT) {
        setBrushPaintType();
    } else {
        RuntimeThrow("Invalid fill type.");
    }

    PaintType currentPaintType;
    Gradient *currentGradient;
    Gradient::Type currentGradientType;
    if(mTarget == PaintSetting::FILL) {
        currentPaintType = mCurrentFillPaintType;
        currentGradient = mCurrentFillGradient;
        currentGradientType = mCurrentFillGradientType;
    } else {
        currentPaintType = mCurrentStrokePaintType;
        currentGradient = mCurrentStrokeGradient;
        currentGradientType = mCurrentStrokeGradientType;
    }
    PaintSettingsApplier paintSetting;
    if(currentPaintType == FLATPAINT) {
        paintSetting << std::make_shared<ColorPaintSetting>(mTarget, ColorSetting());
    } else if(currentPaintType == GRADIENTPAINT) {
        paintSetting << std::make_shared<GradientPaintSetting>(mTarget, currentGradient);
        paintSetting << std::make_shared<GradientTypePaintSetting>(mTarget, currentGradientType);
    } else if(currentPaintType == BRUSHPAINT) {
        paintSetting << std::make_shared<ColorPaintSetting>(mTarget, ColorSetting());
    }
    paintSetting << std::make_shared<PaintTypePaintSetting>(mTarget, currentPaintType);
    mCanvasWindow->applyPaintSettingToSelected(paintSetting);
}

void FillStrokeSettingsWidget::colorSettingReceived(
        const ColorSetting &colorSetting) {
    PaintSettingsApplier paintSetting;
    paintSetting << std::make_shared<ColorPaintSetting>(mTarget, colorSetting);
    mCanvasWindow->applyPaintSettingToSelected(paintSetting);
    mCanvasWindow->setCurrentBrushColor(colorSetting.getColor());
}

void FillStrokeSettingsWidget::connectGradient() {
    connect(mGradientWidget,
            &GradientWidget::selectedColorChanged,
            mColorsSettingsWidget,
            &ColorSettingsWidget::setColorAnimatorTarget);
    connect(mGradientWidget,
            &GradientWidget::currentGradientChanged,
            this, &FillStrokeSettingsWidget::setGradient);

}

void FillStrokeSettingsWidget::disconnectGradient() {
    disconnect(mGradientWidget,
               &GradientWidget::selectedColorChanged,
               mColorsSettingsWidget,
               &ColorSettingsWidget::setColorAnimatorTarget);
    disconnect(mGradientWidget, &GradientWidget::currentGradientChanged,
               this, &FillStrokeSettingsWidget::setGradient);
}

void FillStrokeSettingsWidget::setColorAnimatorTarget(
        ColorAnimator *animator) {
    mColorsSettingsWidget->setColorAnimatorTarget(animator);
}

void FillStrokeSettingsWidget::setJoinStyle(Qt::PenJoinStyle joinStyle) {
    mCurrentJoinStyle = joinStyle;
    mBevelJoinStyleButton->setChecked(joinStyle == Qt::BevelJoin);
    mMiterJointStyleButton->setChecked(joinStyle == Qt::MiterJoin);
    mRoundJoinStyleButton->setChecked(joinStyle == Qt::RoundJoin);
}

void FillStrokeSettingsWidget::setCapStyle(Qt::PenCapStyle capStyle) {
    mCurrentCapStyle = capStyle;
    mFlatCapStyleButton->setChecked(capStyle == Qt::FlatCap);
    mSquareCapStyleButton->setChecked(capStyle == Qt::SquareCap);
    mRoundCapStyleButton->setChecked(capStyle == Qt::RoundCap);
}

PaintType FillStrokeSettingsWidget::getCurrentPaintTypeVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillPaintType;
    else return mCurrentStrokePaintType;
}

void FillStrokeSettingsWidget::setCurrentPaintTypeVal(const PaintType& paintType) {
    if(mTarget == PaintSetting::FILL) mCurrentFillPaintType = paintType;
    else mCurrentStrokePaintType = paintType;
}

QColor FillStrokeSettingsWidget::getCurrentColorVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillColor;
    else return mCurrentStrokeColor;
}

void FillStrokeSettingsWidget::setCurrentColorVal(const QColor& color) {
    if(mTarget == PaintSetting::FILL) mCurrentFillColor = color;
    else mCurrentStrokeColor = color;
}

Gradient *FillStrokeSettingsWidget::getCurrentGradientVal() {
    if(mTarget == PaintSetting::FILL) return mCurrentFillGradient;
    else return mCurrentStrokeGradient;
}

void FillStrokeSettingsWidget::setCurrentGradientVal(Gradient *gradient) {
    if(mTarget == PaintSetting::FILL) mCurrentFillGradient = gradient;
    else mCurrentStrokeGradient = gradient;
}

void FillStrokeSettingsWidget::setCurrentGradientTypeVal(
                                const Gradient::Type &type) {
    if(mTarget == PaintSetting::FILL) mCurrentFillGradientType = type;
    else mCurrentStrokeGradientType = type;
}

void FillStrokeSettingsWidget::setFillValuesFromFillSettings(
        PaintSettingsAnimator *settings) {
    if(settings) {
        mCurrentFillGradientType = settings->getGradientType();
        mCurrentFillColor = settings->getCurrentColor();
        mCurrentFillColorAnimator = settings->getColorAnimator();
        mCurrentFillGradient = settings->getGradient();
        mCurrentFillPaintType = settings->getPaintType();
    } else {
        mCurrentFillColorAnimator = nullptr;
    }
}

void FillStrokeSettingsWidget::setStrokeValuesFromStrokeSettings(
        OutlineSettingsAnimator *settings) {
    if(settings) {
        mCurrentStrokeGradientType = settings->getGradientType();
        mCurrentStrokeColor = settings->getCurrentColor();
        mCurrentStrokeColorAnimator = settings->getColorAnimator();
        mCurrentStrokeGradient = settings->getGradient();
        setCurrentBrushSettings(settings->getBrushSettings());
        mCurrentStrokePaintType = settings->getPaintType();
        mCurrentStrokeWidth = settings->getCurrentStrokeWidth();
        mLineWidthSpin->setTarget(settings->getLineWidthAnimator());
        mCurrentCapStyle = settings->getCapStyle();
        mCurrentJoinStyle = settings->getJoinStyle();
    } else {
        setCurrentBrushSettings(nullptr);
        mCurrentStrokeColorAnimator = nullptr;
        mLineWidthSpin->clearTarget();
    }
}

void FillStrokeSettingsWidget::setCanvasWindowPtr(CanvasWindow *canvasWidget) {
    mCanvasWindow = canvasWidget;
    connect(mLineWidthSpin, &QrealAnimatorValueSlider::editingStarted,
            mCanvasWindow, &CanvasWindow::startSelectedStrokeWidthTransform);
}

void FillStrokeSettingsWidget::emitStrokeBrushChanged() {
    mCanvasWindow->strokeBrushChanged(mCurrentStrokeBrush);
}

void FillStrokeSettingsWidget::emitStrokeBrushWidthCurveChanged() {
    mCanvasWindow->strokeBrushWidthCurveChanged(mCurrentStrokeBrushWidthCurve);
}

void FillStrokeSettingsWidget::emitStrokeBrushTimeCurveChanged() {
    mCanvasWindow->strokeBrushTimeCurveChanged(mCurrentStrokeBrushTimeCurve);
}

void FillStrokeSettingsWidget::emitStrokeBrushSpacingCurveChanged() {
    mCanvasWindow->strokeBrushSpacingCurveChanged(mCurrentStrokeBrushSpacingCurve);
}

void FillStrokeSettingsWidget::emitStrokeBrushPressureCurveChanged() {
    mCanvasWindow->strokeBrushPressureCurveChanged(mCurrentStrokeBrushPressureCurve);
}

void FillStrokeSettingsWidget::emitStrokeWidthChanged() {
    mCanvasWindow->strokeWidthChanged(mCurrentStrokeWidth);
}

void FillStrokeSettingsWidget::emitStrokeWidthChangedTMP() {
    mCanvasWindow->strokeWidthChanged(mCurrentStrokeWidth);
}

void FillStrokeSettingsWidget::emitCapStyleChanged() {
    mCanvasWindow->strokeCapStyleChanged(mCurrentCapStyle);
}

void FillStrokeSettingsWidget::emitJoinStyleChanged() {
    mCanvasWindow->strokeJoinStyleChanged(mCurrentJoinStyle);
}

void FillStrokeSettingsWidget::applyGradient() {
    Gradient *currentGradient;
    Gradient::Type currentGradientType;
    if(mTarget == PaintSetting::FILL) {
        currentGradient = mCurrentFillGradient;
        currentGradientType = mCurrentFillGradientType;
    } else {
        currentGradient = mCurrentStrokeGradient;
        currentGradientType = mCurrentStrokeGradientType;
    }
    PaintSettingsApplier applier;
    applier << std::make_shared<GradientPaintSetting>(mTarget, currentGradient) <<
               std::make_shared<GradientTypePaintSetting>(mTarget, currentGradientType) <<
               std::make_shared<PaintTypePaintSetting>(mTarget, PaintType::GRADIENTPAINT);

    mCanvasWindow->applyPaintSettingToSelected(applier);
}

void FillStrokeSettingsWidget::setGradient(Gradient *gradient) {
    setCurrentGradientVal(gradient);
    applyGradient();
}

void FillStrokeSettingsWidget::setGradientType(const Gradient::Type &type) {
    setCurrentGradientTypeVal(type);
    mLinearGradientButton->setChecked(type == Gradient::LINEAR);
    mRadialGradientButton->setChecked(type == Gradient::RADIAL);
    applyGradient();
}

void FillStrokeSettingsWidget::setBevelJoinStyle() {
    setJoinStyle(Qt::BevelJoin);
    emitJoinStyleChanged();
}

void FillStrokeSettingsWidget::setMiterJoinStyle() {
    setJoinStyle(Qt::MiterJoin);
    emitJoinStyleChanged();
}

void FillStrokeSettingsWidget::setRoundJoinStyle() {
    setJoinStyle(Qt::RoundJoin);
    emitJoinStyleChanged();
}

void FillStrokeSettingsWidget::setFlatCapStyle() {
    setCapStyle(Qt::FlatCap);
    emitCapStyleChanged();
}

void FillStrokeSettingsWidget::setSquareCapStyle() {
    setCapStyle(Qt::SquareCap);
    emitCapStyleChanged();
}

void FillStrokeSettingsWidget::setRoundCapStyle() {
    setCapStyle(Qt::RoundCap);
    emitCapStyleChanged();
}

void FillStrokeSettingsWidget::setFillTarget() {
    mTarget = PaintSetting::FILL;
    mFillBrushButton->hide();
    mFillTargetButton->setChecked(true);
    mStrokeTargetButton->setChecked(false);
    mStrokeSettingsWidget->hide();
    updateAfterTargetChanged();
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setStrokeTarget() {
    mTarget = PaintSetting::OUTLINE;
    mFillBrushButton->show();
    mStrokeTargetButton->setChecked(true);
    mFillTargetButton->setChecked(false);
    mStrokeSettingsWidget->show();
    updateAfterTargetChanged();
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setBrushPaintType() {
    disconnectGradient();
    mColorsSettingsWidget->show();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->show();
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->hide();
    setCurrentPaintTypeVal(BRUSHPAINT);
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setNoPaintType() {
    setCurrentPaintTypeVal(NOPAINT);
    mColorsSettingsWidget->hide();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->hide();
    setCurrentIndex(0);
    updateColorAnimator();
}

void FillStrokeSettingsWidget::setFlatPaintType() {
    disconnectGradient();
    mColorsSettingsWidget->show();
    mGradientWidget->hide();
    mGradientTypeWidget->hide();
    tabBar()->hide();
    setCurrentIndex(0);
    setCurrentPaintTypeVal(FLATPAINT);
    updateColorAnimator();
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
}

void FillStrokeSettingsWidget::setGradientPaintType() {
    connectGradient();
    if(mTarget == PaintSetting::FILL) {
        mCurrentFillPaintType = GRADIENTPAINT;
        if(!mCurrentFillGradient) {
            mCurrentFillGradient = mGradientWidget->getCurrentGradient();
        }
    } else {
        mCurrentStrokePaintType = GRADIENTPAINT;
        if(!mCurrentStrokeGradient) {
            mCurrentStrokeGradient = mGradientWidget->getCurrentGradient();
        }
    }
    if(mColorsSettingsWidget->isHidden()) mColorsSettingsWidget->show();
    if(mGradientWidget->isHidden()) mGradientWidget->show();
    if(mGradientTypeWidget->isHidden()) mGradientTypeWidget->show();
    tabBar()->hide();
    setCurrentIndex(0);
    updateColorAnimator();

    mGradientWidget->update();
    if(mTarget == PaintSetting::OUTLINE) mStrokeJoinCapWidget->show();
}
