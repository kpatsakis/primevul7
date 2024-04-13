void GfxGouraudTriangleShading::getTriangle(int i, double *x0, double *y0, double *color0, double *x1, double *y1, double *color1, double *x2, double *y2, double *color2)
{
    int v;

    assert(isParameterized());

    v = triangles[i][0];
    if (likely(v >= 0 && v < nVertices)) {
        *x0 = vertices[v].x;
        *y0 = vertices[v].y;
        *color0 = colToDbl(vertices[v].color.c[0]);
    }
    v = triangles[i][1];
    if (likely(v >= 0 && v < nVertices)) {
        *x1 = vertices[v].x;
        *y1 = vertices[v].y;
        *color1 = colToDbl(vertices[v].color.c[0]);
    }
    v = triangles[i][2];
    if (likely(v >= 0 && v < nVertices)) {
        *x2 = vertices[v].x;
        *y2 = vertices[v].y;
        *color2 = colToDbl(vertices[v].color.c[0]);
    }
}