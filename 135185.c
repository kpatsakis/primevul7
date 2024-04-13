GfxAxialShading *GfxAxialShading::parse(GfxResources *res, Dict *dict, OutputDev *out, GfxState *state)
{
    GfxAxialShading *shading;
    double x0A, y0A, x1A, y1A;
    double t0A, t1A;
    std::vector<std::unique_ptr<Function>> funcsA;
    bool extend0A, extend1A;
    Object obj1;

    x0A = y0A = x1A = y1A = 0;
    obj1 = dict->lookup("Coords");
    if (obj1.isArray() && obj1.arrayGetLength() == 4) {
        x0A = obj1.arrayGet(0).getNumWithDefaultValue(0);
        y0A = obj1.arrayGet(1).getNumWithDefaultValue(0);
        x1A = obj1.arrayGet(2).getNumWithDefaultValue(0);
        y1A = obj1.arrayGet(3).getNumWithDefaultValue(0);
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
        if (nFuncsA > gfxColorMaxComps || nFuncsA == 0) {
            error(errSyntaxWarning, -1, "Invalid Function array in shading dictionary");
            return nullptr;
        }
        for (int i = 0; i < nFuncsA; ++i) {
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
        Object obj2 = obj1.arrayGet(0);
        if (obj2.isBool()) {
            extend0A = obj2.getBool();
        } else {
            error(errSyntaxWarning, -1, "Invalid axial shading extend (0)");
        }
        obj2 = obj1.arrayGet(1);
        if (obj2.isBool()) {
            extend1A = obj2.getBool();
        } else {
            error(errSyntaxWarning, -1, "Invalid axial shading extend (1)");
        }
    }

    shading = new GfxAxialShading(x0A, y0A, x1A, y1A, t0A, t1A, std::move(funcsA), extend0A, extend1A);
    if (!shading->init(res, dict, out, state)) {
        delete shading;
        shading = nullptr;
    }
    return shading;
}