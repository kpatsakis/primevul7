GfxGouraudTriangleShading::GfxGouraudTriangleShading(int typeA, GfxGouraudVertex *verticesA, int nVerticesA, int (*trianglesA)[3], int nTrianglesA, std::vector<std::unique_ptr<Function>> &&funcsA)
    : GfxShading(typeA), funcs(std::move(funcsA))
{
    vertices = verticesA;
    nVertices = nVerticesA;
    triangles = trianglesA;
    nTriangles = nTrianglesA;
}