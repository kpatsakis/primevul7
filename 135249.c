void GfxState::getUserClipBBox(double *xMin, double *yMin, double *xMax, double *yMax) const
{
    double ictm[6];
    double xMin1, yMin1, xMax1, yMax1, tx, ty;

    // invert the CTM
    const double det_denominator = (ctm[0] * ctm[3] - ctm[1] * ctm[2]);
    if (unlikely(det_denominator == 0)) {
        *xMin = 0;
        *yMin = 0;
        *xMax = 0;
        *yMax = 0;
        return;
    }
    const double det = 1 / det_denominator;
    ictm[0] = ctm[3] * det;
    ictm[1] = -ctm[1] * det;
    ictm[2] = -ctm[2] * det;
    ictm[3] = ctm[0] * det;
    ictm[4] = (ctm[2] * ctm[5] - ctm[3] * ctm[4]) * det;
    ictm[5] = (ctm[1] * ctm[4] - ctm[0] * ctm[5]) * det;

    // transform all four corners of the clip bbox; find the min and max
    // x and y values
    xMin1 = xMax1 = clipXMin * ictm[0] + clipYMin * ictm[2] + ictm[4];
    yMin1 = yMax1 = clipXMin * ictm[1] + clipYMin * ictm[3] + ictm[5];
    tx = clipXMin * ictm[0] + clipYMax * ictm[2] + ictm[4];
    ty = clipXMin * ictm[1] + clipYMax * ictm[3] + ictm[5];
    if (tx < xMin1) {
        xMin1 = tx;
    } else if (tx > xMax1) {
        xMax1 = tx;
    }
    if (ty < yMin1) {
        yMin1 = ty;
    } else if (ty > yMax1) {
        yMax1 = ty;
    }
    tx = clipXMax * ictm[0] + clipYMin * ictm[2] + ictm[4];
    ty = clipXMax * ictm[1] + clipYMin * ictm[3] + ictm[5];
    if (tx < xMin1) {
        xMin1 = tx;
    } else if (tx > xMax1) {
        xMax1 = tx;
    }
    if (ty < yMin1) {
        yMin1 = ty;
    } else if (ty > yMax1) {
        yMax1 = ty;
    }
    tx = clipXMax * ictm[0] + clipYMax * ictm[2] + ictm[4];
    ty = clipXMax * ictm[1] + clipYMax * ictm[3] + ictm[5];
    if (tx < xMin1) {
        xMin1 = tx;
    } else if (tx > xMax1) {
        xMax1 = tx;
    }
    if (ty < yMin1) {
        yMin1 = ty;
    } else if (ty > yMax1) {
        yMax1 = ty;
    }

    *xMin = xMin1;
    *yMin = yMin1;
    *xMax = xMax1;
    *yMax = yMax1;
}