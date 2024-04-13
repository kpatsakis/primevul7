void GfxCalGrayColorSpace::getXYZ(const GfxColor *color, double *pX, double *pY, double *pZ) const
{
    const double A = colToDbl(color->c[0]);
    const double xyzColor = pow(A, gamma);
    *pX = xyzColor;
    *pY = xyzColor;
    *pZ = xyzColor;
}