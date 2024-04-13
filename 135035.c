GfxFunctionShading::GfxFunctionShading(const GfxFunctionShading *shading) : GfxShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    x1 = shading->x1;
    y1 = shading->y1;
    for (int i = 0; i < 6; ++i) {
        matrix[i] = shading->matrix[i];
    }
    for (const auto &f : shading->funcs) {
        funcs.emplace_back(f->copy());
    }
}