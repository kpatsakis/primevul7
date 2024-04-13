void GfxPath::curveTo(double x1, double y1, double x2, double y2, double x3, double y3)
{
    if (justMoved || (n > 0 && subpaths[n - 1]->isClosed())) {
        if (n >= size) {
            size *= 2;
            subpaths = (GfxSubpath **)greallocn(subpaths, size, sizeof(GfxSubpath *));
        }
        if (justMoved) {
            subpaths[n] = new GfxSubpath(firstX, firstY);
        } else {
            subpaths[n] = new GfxSubpath(subpaths[n - 1]->getLastX(), subpaths[n - 1]->getLastY());
        }
        ++n;
        justMoved = false;
    }
    subpaths[n - 1]->curveTo(x1, y1, x2, y2, x3, y3);
}