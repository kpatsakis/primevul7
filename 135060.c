void GfxPath::lineTo(double x, double y)
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
    subpaths[n - 1]->lineTo(x, y);
}