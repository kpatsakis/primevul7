GfxFunctionShading *GfxFunctionShading::parse(GfxResources *res, Dict *dict, OutputDev *out, GfxState *state)
{
    GfxFunctionShading *shading;
    double x0A, y0A, x1A, y1A;
    double matrixA[6];
    std::vector<std::unique_ptr<Function>> funcsA;
    Object obj1;
    int i;

    x0A = y0A = 0;
    x1A = y1A = 1;
    obj1 = dict->lookup("Domain");
    if (obj1.isArray() && obj1.arrayGetLength() == 4) {
        bool decodeOk = true;
        x0A = obj1.arrayGet(0).getNum(&decodeOk);
        x1A = obj1.arrayGet(1).getNum(&decodeOk);
        y0A = obj1.arrayGet(2).getNum(&decodeOk);
        y1A = obj1.arrayGet(3).getNum(&decodeOk);

        if (!decodeOk) {
            error(errSyntaxWarning, -1, "Invalid Domain array in function shading dictionary");
            return nullptr;
        }
    }

    matrixA[0] = 1;
    matrixA[1] = 0;
    matrixA[2] = 0;
    matrixA[3] = 1;
    matrixA[4] = 0;
    matrixA[5] = 0;
    obj1 = dict->lookup("Matrix");
    if (obj1.isArray() && obj1.arrayGetLength() == 6) {
        bool decodeOk = true;
        matrixA[0] = obj1.arrayGet(0).getNum(&decodeOk);
        matrixA[1] = obj1.arrayGet(1).getNum(&decodeOk);
        matrixA[2] = obj1.arrayGet(2).getNum(&decodeOk);
        matrixA[3] = obj1.arrayGet(3).getNum(&decodeOk);
        matrixA[4] = obj1.arrayGet(4).getNum(&decodeOk);
        matrixA[5] = obj1.arrayGet(5).getNum(&decodeOk);

        if (!decodeOk) {
            error(errSyntaxWarning, -1, "Invalid Matrix array in function shading dictionary");
            return nullptr;
        }
    }

    obj1 = dict->lookup("Function");
    if (obj1.isArray()) {
        const int nFuncsA = obj1.arrayGetLength();
        if (nFuncsA > gfxColorMaxComps || nFuncsA <= 0) {
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

    shading = new GfxFunctionShading(x0A, y0A, x1A, y1A, matrixA, std::move(funcsA));
    if (!shading->init(res, dict, out, state)) {
        delete shading;
        return nullptr;
    }
    return shading;
}