GfxRadialShading *GfxRadialShading::parse(GfxResources *res, Dict *dict, OutputDev *out, GfxState *state)
{
    GfxRadialShading *shading;
    double x0A, y0A, r0A, x1A, y1A, r1A;
    double t0A, t1A;
    std::vector<std::unique_ptr<Function>> funcsA;
    bool extend0A, extend1A;
    Object obj1;
    int i;

    x0A = y0A = r0A = x1A = y1A = r1A = 0;
    obj1 = dict->lookup("Coords");
    if (obj1.isArray() && obj1.arrayGetLength() == 6) {
        x0A = obj1.arrayGet(0).getNumWithDefaultValue(0);
        y0A = obj1.arrayGet(1).getNumWithDefaultValue(0);
        r0A = obj1.arrayGet(2).getNumWithDefaultValue(0);
        x1A = obj1.arrayGet(3).getNumWithDefaultValue(0);
        y1A = obj1.arrayGet(4).getNumWithDefaultValue(0);
        r1A = obj1.arrayGet(5).getNumWithDefaultValue(0);
    } else {
        error(errSyntaxWarning, -1, "Missing or invalid Coords in shading dictionary");
        return nullptr;
    }

    t0A = 0;
    t1A = 1;
    obj1 = dict->lookup("Domain");
    if (obj1.isArray() && obj1.arrayGetLength() == 2) {
        t0A = obj1.arrayGet(0).getNumWithDefaultValue(0);
        t1A = obj1.arrayGet(1).getNumWithDefaultValue(1);
    }

    obj1 = dict->lookup("Function");
    if (obj1.isArray()) {
        const int nFuncsA = obj1.arrayGetLength();
        if (nFuncsA > gfxColorMaxComps) {
            error(errSyntaxWarning, -1, "Invalid Function array in shading dictionary");
            return nullptr;
        }
        for (i = 0; i < nFuncsA; ++i) {
            Object obj2 = obj1.arrayGet(i);
            Function *f = Function::parse(&obj2);
            if (!f) {
                return nullptr;
            }
            funcsA.emplace_back(f);
        }
    } else {
        Function *f = Function::parse(&obj1);
        if (!f) {
            return nullptr;
        }
        funcsA.emplace_back(f);
    }

    extend0A = extend1A = false;
    obj1 = dict->lookup("Extend");
    if (obj1.isArray() && obj1.arrayGetLength() == 2) {
        extend0A = obj1.arrayGet(0).getBoolWithDefaultValue(false);
        extend1A = obj1.arrayGet(1).getBoolWithDefaultValue(false);
    }

    shading = new GfxRadialShading(x0A, y0A, r0A, x1A, y1A, r1A, t0A, t1A, std::move(funcsA), extend0A, extend1A);
    if (!shading->init(res, dict, out, state)) {
        delete shading;
        return nullptr;
    }
    return shading;
}