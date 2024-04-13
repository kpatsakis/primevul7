GfxSubpath::GfxSubpath(const GfxSubpath *subpath)
{
    size = subpath->size;
    n = subpath->n;
    x = (double *)gmallocn(size, sizeof(double));
    y = (double *)gmallocn(size, sizeof(double));
    curve = (bool *)gmallocn(size, sizeof(bool));
    memcpy(x, subpath->x, n * sizeof(double));
    memcpy(y, subpath->y, n * sizeof(double));
    memcpy(curve, subpath->curve, n * sizeof(bool));
    closed = subpath->closed;
}