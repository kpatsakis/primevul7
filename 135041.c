void GfxState::clipToRect(double xMin, double yMin, double xMax, double yMax)
{
    double x, y, xMin1, yMin1, xMax1, yMax1;

    transform(xMin, yMin, &x, &y);
    xMin1 = xMax1 = x;
    yMin1 = yMax1 = y;
    transform(xMax, yMin, &x, &y);
    if (x < xMin1) {
        xMin1 = x;
    } else if (x > xMax1) {
        xMax1 = x;
    }
    if (y < yMin1) {
        yMin1 = y;
    } else if (y > yMax1) {
        yMax1 = y;
    }
    transform(xMax, yMax, &x, &y);
    if (x < xMin1) {
        xMin1 = x;
    } else if (x > xMax1) {
        xMax1 = x;
    }
    if (y < yMin1) {
        yMin1 = y;
    } else if (y > yMax1) {
        yMax1 = y;
    }
    transform(xMin, yMax, &x, &y);
    if (x < xMin1) {
        xMin1 = x;
    } else if (x > xMax1) {
        xMax1 = x;
    }
    if (y < yMin1) {
        yMin1 = y;
    } else if (y > yMax1) {
        yMax1 = y;
    }

    if (xMin1 > clipXMin) {
        clipXMin = xMin1;
    }
    if (yMin1 > clipYMin) {
        clipYMin = yMin1;
    }
    if (xMax1 < clipXMax) {
        clipXMax = xMax1;
    }
    if (yMax1 < clipYMax) {
        clipYMax = yMax1;
    }
}