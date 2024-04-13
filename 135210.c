GfxAxialShading::GfxAxialShading(const GfxAxialShading *shading) : GfxUnivariateShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    x1 = shading->x1;
    y1 = shading->y1;
}