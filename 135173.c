GfxShadingPattern::GfxShadingPattern(GfxShading *shadingA, const double *matrixA, int patternRefNumA) : GfxPattern(2, patternRefNumA)
{
    int i;

    shading = shadingA;
    for (i = 0; i < 6; ++i) {
        matrix[i] = matrixA[i];
    }
}