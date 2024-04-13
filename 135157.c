GfxGouraudTriangleShading::~GfxGouraudTriangleShading()
{
    gfree(vertices);
    gfree(triangles);
}