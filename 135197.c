void GfxPath::close()
{
    // this is necessary to handle the pathological case of
    // moveto/closepath/clip, which defines an empty clipping region
    if (justMoved) {
        if (n >= size) {
            size *= 2;
            subpaths = (GfxSubpath **)greallocn(subpaths, size, sizeof(GfxSubpath *));
        }
        subpaths[n] = new GfxSubpath(firstX, firstY);
        ++n;
        justMoved = false;
    }
    subpaths[n - 1]->close();
}