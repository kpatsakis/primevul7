void GfxSubpath::lineTo(double x1, double y1)
{
    if (n >= size) {
        size *= 2;
        x = (double *)greallocn(x, size, sizeof(double));
        y = (double *)greallocn(y, size, sizeof(double));
        curve = (bool *)greallocn(curve, size, sizeof(bool));
    }
    x[n] = x1;
    y[n] = y1;
    curve[n] = false;
    ++n;
}