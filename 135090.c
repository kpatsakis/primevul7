GfxUnivariateShading::GfxUnivariateShading(int typeA, double t0A, double t1A, std::vector<std::unique_ptr<Function>> &&funcsA, bool extend0A, bool extend1A) : GfxShading(typeA), funcs(std::move(funcsA))
{
    t0 = t0A;
    t1 = t1A;
    extend0 = extend0A;
    extend1 = extend1A;

    cacheSize = 0;
    lastMatch = 0;
    cacheBounds = nullptr;
    cacheCoeff = nullptr;
    cacheValues = nullptr;
}