GfxAxialShading::GfxAxialShading(double x0A, double y0A, double x1A, double y1A, double t0A, double t1A, std::vector<std::unique_ptr<Function>> &&funcsA, bool extend0A, bool extend1A)
    : GfxUnivariateShading(2, t0A, t1A, std::move(funcsA), extend0A, extend1A)
{
    x0 = x0A;
    y0 = y0A;
    x1 = x1A;
    y1 = y1A;
}