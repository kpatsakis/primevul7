GfxState::GfxState(const GfxState *state, bool copyPath)
{
    int i;

    hDPI = state->hDPI;
    vDPI = state->vDPI;
    memcpy(ctm, state->ctm, sizeof(ctm));
    px1 = state->px1;
    py1 = state->py1;
    px2 = state->px2;
    py2 = state->py2;
    pageWidth = state->pageWidth;
    pageHeight = state->pageHeight;
    rotate = state->rotate;

    fillColorSpace = state->fillColorSpace;
    if (fillColorSpace) {
        fillColorSpace = state->fillColorSpace->copy();
    }
    strokeColorSpace = state->strokeColorSpace;
    if (strokeColorSpace) {
        strokeColorSpace = state->strokeColorSpace->copy();
    }
    fillColor = state->fillColor;
    strokeColor = state->strokeColor;

    fillPattern = state->fillPattern;
    if (fillPattern) {
        fillPattern = state->fillPattern->copy();
    }
    strokePattern = state->strokePattern;
    if (strokePattern) {
        strokePattern = state->strokePattern->copy();
    }
    blendMode = state->blendMode;
    fillOpacity = state->fillOpacity;
    strokeOpacity = state->strokeOpacity;
    fillOverprint = state->fillOverprint;
    strokeOverprint = state->strokeOverprint;
    overprintMode = state->overprintMode;
    for (i = 0; i < 4; ++i) {
        transfer[i] = state->transfer[i];
        if (transfer[i]) {
            transfer[i] = state->transfer[i]->copy();
        }
    }
    lineWidth = state->lineWidth;
    lineDashLength = state->lineDashLength;
    lineDash = nullptr;
    if (lineDashLength > 0) {
        lineDash = (double *)gmallocn(lineDashLength, sizeof(double));
        memcpy(lineDash, state->lineDash, lineDashLength * sizeof(double));
    }
    lineDashStart = state->lineDashStart;
    flatness = state->flatness;
    lineJoin = state->lineJoin;
    lineCap = state->lineCap;
    miterLimit = state->miterLimit;
    strokeAdjust = state->strokeAdjust;
    alphaIsShape = state->alphaIsShape;
    textKnockout = state->textKnockout;

    font = state->font;
    if (font)
        font->incRefCnt();
    fontSize = state->fontSize;
    memcpy(textMat, state->textMat, sizeof(textMat));
    charSpace = state->charSpace;
    wordSpace = state->wordSpace;
    horizScaling = state->horizScaling;
    leading = state->leading;
    rise = state->rise;
    render = state->render;

    path = state->path;
    if (copyPath) {
        path = state->path->copy();
    }
    curX = state->curX;
    curY = state->curY;
    lineX = state->lineX;
    lineY = state->lineY;

    clipXMin = state->clipXMin;
    clipYMin = state->clipYMin;
    clipXMax = state->clipXMax;
    clipYMax = state->clipYMax;
    memcpy(renderingIntent, state->renderingIntent, sizeof(renderingIntent));

    saved = nullptr;
#ifdef USE_CMS
    localDisplayProfile = state->localDisplayProfile;
    XYZ2DisplayTransformRelCol = state->XYZ2DisplayTransformRelCol;
    XYZ2DisplayTransformAbsCol = state->XYZ2DisplayTransformAbsCol;
    XYZ2DisplayTransformSat = state->XYZ2DisplayTransformSat;
    XYZ2DisplayTransformPerc = state->XYZ2DisplayTransformPerc;
#endif
}