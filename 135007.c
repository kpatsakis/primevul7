GfxRadialShading::GfxRadialShading(double x0A, double y0A, double r0A, double x1A, double y1A, double r1A, double t0A, double t1A, std::vector<std::unique_ptr<Function>> &&funcsA, bool extend0A, bool extend1A)
    : GfxUnivariateShading(3, t0A, t1A, std::move(funcsA), extend0A, extend1A)
{
    x0 = x0A;
    y0 = y0A;
    r0 = r0A;
    x1 = x1A;
    y1 = y1A;
    r1 = r1A;
}