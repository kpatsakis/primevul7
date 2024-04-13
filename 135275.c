double GfxRadialShading::getDistance(double sMin, double sMax) const
{
    double xMin, yMin, rMin, xMax, yMax, rMax;

    xMin = x0 + sMin * (x1 - x0);
    yMin = y0 + sMin * (y1 - y0);
    rMin = r0 + sMin * (r1 - r0);

    xMax = x0 + sMax * (x1 - x0);
    yMax = y0 + sMax * (y1 - y0);
    rMax = r0 + sMax * (r1 - r0);

    return hypot(xMax - xMin, yMax - yMin) + fabs(rMax - rMin);
}