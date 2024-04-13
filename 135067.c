double GfxState::transformWidth(double w) const
{
    double x, y;

    x = ctm[0] + ctm[2];
    y = ctm[1] + ctm[3];
    return w * sqrt(0.5 * (x * x + y * y));
}