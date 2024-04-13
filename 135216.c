GfxPath::GfxPath()
{
    justMoved = false;
    size = 16;
    n = 0;
    firstX = firstY = 0;
    subpaths = (GfxSubpath **)gmallocn(size, sizeof(GfxSubpath *));
}