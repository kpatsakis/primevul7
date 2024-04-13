void GfxGouraudTriangleShading::getTriangle(int i, double *x0, double *y0, GfxColor *color0, double *x1, double *y1, GfxColor *color1, double *x2, double *y2, GfxColor *color2)
{
    int v;

    assert(!isParameterized());

    v = triangles[i][0];
    *x0 = vertices[v].x;
    *y0 = vertices[v].y;
    *color0 = vertices[v].color;
    v = triangles[i][1];
    *x1 = vertices[v].x;
    *y1 = vertices[v].y;
    *color1 = vertices[v].color;
    v = triangles[i][2];
    *x2 = vertices[v].x;
    *y2 = vertices[v].y;
    *color2 = vertices[v].color;
}