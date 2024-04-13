GfxSubpath::GfxSubpath(double x1, double y1)
{
    size = 16;
    x = (double *)gmallocn(size, sizeof(double));
    y = (double *)gmallocn(size, sizeof(double));
    curve = (bool *)gmallocn(size, sizeof(bool));
    n = 1;
    x[0] = x1;
    y[0] = y1;
    curve[0] = false;
    closed = false;
}