void GfxSubpath::curveTo(double x1, double y1, double x2, double y2, double x3, double y3)
{
    if (n + 3 > size) {
        size *= 2;
        x = (double *)greallocn(x, size, sizeof(double));
        y = (double *)greallocn(y, size, sizeof(double));
        curve = (bool *)greallocn(curve, size, sizeof(bool));
    }
    x[n] = x1;
    y[n] = y1;
    x[n + 1] = x2;
    y[n + 1] = y2;
    x[n + 2] = x3;
    y[n + 2] = y3;
    curve[n] = curve[n + 1] = true;
    curve[n + 2] = false;
    n += 3;
}