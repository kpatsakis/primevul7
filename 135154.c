GfxState::GfxState(double hDPIA, double vDPIA, const PDFRectangle *pageBox, int rotateA, bool upsideDown)
{
    double kx, ky;

    hDPI = hDPIA;
    vDPI = vDPIA;
    rotate = rotateA;
    px1 = pageBox->x1;
    py1 = pageBox->y1;
    px2 = pageBox->x2;
    py2 = pageBox->y2;
    kx = hDPI / 72.0;
    ky = vDPI / 72.0;
    if (rotate == 90) {
        ctm[0] = 0;
        ctm[1] = upsideDown ? ky : -ky;
        ctm[2] = kx;
        ctm[3] = 0;
        ctm[4] = -kx * py1;
        ctm[5] = ky * (upsideDown ? -px1 : px2);
        pageWidth = kx * (py2 - py1);
        pageHeight = ky * (px2 - px1);
    } else if (rotate == 180) {
        ctm[0] = -kx;
        ctm[1] = 0;
        ctm[2] = 0;
        ctm[3] = upsideDown ? ky : -ky;
        ctm[4] = kx * px2;
        ctm[5] = ky * (upsideDown ? -py1 : py2);
        pageWidth = kx * (px2 - px1);
        pageHeight = ky * (py2 - py1);
    } else if (rotate == 270) {
        ctm[0] = 0;
        ctm[1] = upsideDown ? -ky : ky;
        ctm[2] = -kx;
        ctm[3] = 0;
        ctm[4] = kx * py2;
        ctm[5] = ky * (upsideDown ? px2 : -px1);
        pageWidth = kx * (py2 - py1);
        pageHeight = ky * (px2 - px1);
    } else {
        ctm[0] = kx;
        ctm[1] = 0;
        ctm[2] = 0;
        ctm[3] = upsideDown ? -ky : ky;
        ctm[4] = -kx * px1;
        ctm[5] = ky * (upsideDown ? py2 : -py1);
        pageWidth = kx * (px2 - px1);
        pageHeight = ky * (py2 - py1);
    }

    fillColorSpace = new GfxDeviceGrayColorSpace();
    strokeColorSpace = new GfxDeviceGrayColorSpace();
    fillColor.c[0] = 0;
    strokeColor.c[0] = 0;
    fillPattern = nullptr;
    strokePattern = nullptr;
    blendMode = gfxBlendNormal;
    fillOpacity = 1;
    strokeOpacity = 1;
    fillOverprint = false;
    strokeOverprint = false;
    overprintMode = 0;
    transfer[0] = transfer[1] = transfer[2] = transfer[3] = nullptr;

    lineWidth = 1;
    lineDash = nullptr;
    lineDashLength = 0;
    lineDashStart = 0;
    flatness = 1;
    lineJoin = 0;
    lineCap = 0;
    miterLimit = 10;
    strokeAdjust = false;
    alphaIsShape = false;
    textKnockout = false;

    font = nullptr;
    fontSize = 0;
    textMat[0] = 1;
    textMat[1] = 0;
    textMat[2] = 0;
    textMat[3] = 1;
    textMat[4] = 0;
    textMat[5] = 0;
    charSpace = 0;
    wordSpace = 0;
    horizScaling = 1;
    leading = 0;
    rise = 0;
    render = 0;

    path = new GfxPath();
    curX = curY = 0;
    lineX = lineY = 0;

    clipXMin = 0;
    clipYMin = 0;
    clipXMax = pageWidth;
    clipYMax = pageHeight;

    renderingIntent[0] = 0;

    saved = nullptr;
#ifdef USE_CMS
    XYZ2DisplayTransformRelCol = nullptr;
    XYZ2DisplayTransformAbsCol = nullptr;
    XYZ2DisplayTransformSat = nullptr;
    XYZ2DisplayTransformPerc = nullptr;
    localDisplayProfile = nullptr;

    if (!sRGBProfile) {
        // This is probably the one of the first invocations of lcms2, so we set the error handler
        cmsSetLogErrorHandler(CMSError);

        sRGBProfile = make_GfxLCMSProfilePtr(cmsCreate_sRGBProfile());
    }

    if (!XYZProfile) {
        XYZProfile = make_GfxLCMSProfilePtr(cmsCreateXYZProfile());
    }
#endif
}