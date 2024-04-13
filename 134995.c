double GfxAxialShading::getDistance(double sMin, double sMax) const
{
    double xMin, yMin, xMax, yMax;

    xMin = x0 + sMin * (x1 - x0);
    yMin = y0 + sMin * (y1 - y0);
    xMax = x0 + sMax * (x1 - x0);
    yMax = y0 + sMax * (y1 - y0);

    return hypot(xMax - xMin, yMax - yMin);
}