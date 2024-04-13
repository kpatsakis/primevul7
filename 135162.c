GfxTilingPattern::GfxTilingPattern(int paintTypeA, int tilingTypeA, const double *bboxA, double xStepA, double yStepA, const Object *resDictA, const double *matrixA, const Object *contentStreamA, int patternRefNumA)
    : GfxPattern(1, patternRefNumA)
{
    int i;

    paintType = paintTypeA;
    tilingType = tilingTypeA;
    for (i = 0; i < 4; ++i) {
        bbox[i] = bboxA[i];
    }
    xStep = xStepA;
    yStep = yStepA;
    resDict = resDictA->copy();
    for (i = 0; i < 6; ++i) {
        matrix[i] = matrixA[i];
    }
    contentStream = contentStreamA->copy();
}