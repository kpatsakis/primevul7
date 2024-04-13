GfxFunctionShading::GfxFunctionShading(double x0A, double y0A, double x1A, double y1A, const double *matrixA, std::vector<std::unique_ptr<Function>> &&funcsA) : GfxShading(1), funcs(std::move(funcsA))
{
    x0 = x0A;
    y0 = y0A;
    x1 = x1A;
    y1 = y1A;
    for (int i = 0; i < 6; ++i) {
        matrix[i] = matrixA[i];
    }
}