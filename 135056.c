GfxGouraudTriangleShading::GfxGouraudTriangleShading(const GfxGouraudTriangleShading *shading) : GfxShading(shading)
{
    nVertices = shading->nVertices;
    vertices = (GfxGouraudVertex *)gmallocn(nVertices, sizeof(GfxGouraudVertex));
    memcpy(vertices, shading->vertices, nVertices * sizeof(GfxGouraudVertex));
    nTriangles = shading->nTriangles;
    triangles = (int(*)[3])gmallocn(nTriangles * 3, sizeof(int));
    memcpy(triangles, shading->triangles, nTriangles * 3 * sizeof(int));
    for (const auto &f : shading->funcs) {
        funcs.emplace_back(f->copy());
    }
}