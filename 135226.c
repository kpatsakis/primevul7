GfxShading::GfxShading(const GfxShading *shading)
{
    int i;

    type = shading->type;
    colorSpace = shading->colorSpace->copy();
    for (i = 0; i < gfxColorMaxComps; ++i) {
        background.c[i] = shading->background.c[i];
    }
    hasBackground = shading->hasBackground;
    bbox_xMin = shading->bbox_xMin;
    bbox_yMin = shading->bbox_yMin;
    bbox_xMax = shading->bbox_xMax;
    bbox_yMax = shading->bbox_yMax;
    hasBBox = shading->hasBBox;
}