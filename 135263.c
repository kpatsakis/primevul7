void GfxFunctionShading::getColor(double x, double y, GfxColor *color) const
{
    double in[2], out[gfxColorMaxComps];

    // NB: there can be one function with n outputs or n functions with
    // one output each (where n = number of color components)
    for (double &i : out) {
        i = 0;
    }
    in[0] = x;
    in[1] = y;
    for (int i = 0; i < getNFuncs(); ++i) {
        if (likely(funcs[i]->getInputSize() <= 2)) {
            funcs[i]->transform(in, &out[i]);
        } else {
            error(errSyntaxWarning, -1, "GfxFunctionShading::getColor: function with input size > 2");
        }
    }
    for (int i = 0; i < gfxColorMaxComps; ++i) {
        color->c[i] = dblToCol(out[i]);
    }
}