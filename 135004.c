void GfxPatchMeshShading::getParameterizedColor(double t, GfxColor *color) const
{
    double out[gfxColorMaxComps] = {};

    for (unsigned int j = 0; j < funcs.size(); ++j) {
        funcs[j]->transform(&t, &out[j]);
    }
    for (int j = 0; j < gfxColorMaxComps; ++j) {
        color->c[j] = dblToCol(out[j]);
    }
}