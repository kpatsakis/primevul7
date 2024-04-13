void GfxCalRGBColorSpace::getXYZ(const GfxColor *color, double *pX, double *pY, double *pZ) const
{
    double A, B, C;

    A = pow(colToDbl(color->c[0]), gammaR);
    B = pow(colToDbl(color->c[1]), gammaG);
    C = pow(colToDbl(color->c[2]), gammaB);
    *pX = mat[0] * A + mat[3] * B + mat[6] * C;
    *pY = mat[1] * A + mat[4] * B + mat[7] * C;
    *pZ = mat[2] * A + mat[5] * B + mat[8] * C;
}