GfxUnivariateShading::GfxUnivariateShading(const GfxUnivariateShading *shading) : GfxShading(shading)
{
    t0 = shading->t0;
    t1 = shading->t1;
    for (const auto &f : shading->funcs) {
        funcs.emplace_back(f->copy());
    }
    extend0 = shading->extend0;
    extend1 = shading->extend1;

    cacheSize = 0;
    lastMatch = 0;
    cacheBounds = nullptr;
    cacheCoeff = nullptr;
    cacheValues = nullptr;
}