GfxRadialShading::GfxRadialShading(const GfxRadialShading *shading) : GfxUnivariateShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    r0 = shading->r0;
    x1 = shading->x1;
    y1 = shading->y1;
    r1 = shading->r1;
}