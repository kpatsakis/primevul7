GfxColorSpace *GfxDeviceNColorSpace::parse(GfxResources *res, Array *arr, OutputDev *out, GfxState *state, int recursion)
{
    int nCompsA;
    std::vector<std::string> namesA;
    GfxColorSpace *altA;
    Function *funcA;
    Object obj1;
    auto separationList = new std::vector<GfxSeparationColorSpace *>();

    if (arr->getLength() != 4 && arr->getLength() != 5) {
        error(errSyntaxWarning, -1, "Bad DeviceN color space");
        goto err1;
    }
    obj1 = arr->get(1);
    if (!obj1.isArray()) {
        error(errSyntaxWarning, -1, "Bad DeviceN color space (names)");
        goto err1;
    }
    nCompsA = obj1.arrayGetLength();
    if (nCompsA > gfxColorMaxComps) {
        error(errSyntaxWarning, -1, "DeviceN color space with too many ({0:d} > {1:d}) components", nCompsA, gfxColorMaxComps);
        nCompsA = gfxColorMaxComps;
    }
    for (int i = 0; i < nCompsA; ++i) {
        Object obj2 = obj1.arrayGet(i);
        if (!obj2.isName()) {
            error(errSyntaxWarning, -1, "Bad DeviceN color space (names)");
            nCompsA = i;
            goto err1;
        }
        namesA.emplace_back(obj2.getName());
    }
    obj1 = arr->get(2);
    if (!(altA = GfxColorSpace::parse(res, &obj1, out, state, recursion + 1))) {
        error(errSyntaxWarning, -1, "Bad DeviceN color space (alternate color space)");
        goto err1;
    }
    obj1 = arr->get(3);
    if (!(funcA = Function::parse(&obj1))) {
        goto err4;
    }
    if (arr->getLength() == 5) {
        obj1 = arr->get(4);
        if (!obj1.isDict()) {
            error(errSyntaxWarning, -1, "Bad DeviceN color space (attributes)");
            goto err5;
        }
        Dict *attribs = obj1.getDict();
        Object obj2 = attribs->lookup("Colorants");
        if (obj2.isDict()) {
            Dict *colorants = obj2.getDict();
            for (int i = 0; i < colorants->getLength(); i++) {
                Object obj3 = colorants->getVal(i);
                if (obj3.isArray()) {
                    GfxSeparationColorSpace *cs = (GfxSeparationColorSpace *)GfxSeparationColorSpace::parse(res, obj3.getArray(), out, state, recursion);
                    if (cs) {
                        separationList->push_back(cs);
                    }
                } else {
                    error(errSyntaxWarning, -1, "Bad DeviceN color space (colorant value entry is not an Array)");
                    goto err5;
                }
            }
        }
    }
    return new GfxDeviceNColorSpace(nCompsA, std::move(namesA), altA, funcA, separationList);

err5:
    delete funcA;
err4:
    delete altA;
err1:
    delete separationList;
    return nullptr;
}