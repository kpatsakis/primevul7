GfxColorSpace *GfxSeparationColorSpace::parse(GfxResources *res, Array *arr, OutputDev *out, GfxState *state, int recursion)
{
    GfxSeparationColorSpace *cs;
    GooString *nameA;
    GfxColorSpace *altA;
    Function *funcA;
    Object obj1;

    if (arr->getLength() != 4) {
        error(errSyntaxWarning, -1, "Bad Separation color space");
        goto err1;
    }
    obj1 = arr->get(1);
    if (!obj1.isName()) {
        error(errSyntaxWarning, -1, "Bad Separation color space (name)");
        goto err1;
    }
    nameA = new GooString(obj1.getName());
    obj1 = arr->get(2);
    if (!(altA = GfxColorSpace::parse(res, &obj1, out, state, recursion + 1))) {
        error(errSyntaxWarning, -1, "Bad Separation color space (alternate color space)");
        goto err3;
    }
    obj1 = arr->get(3);
    if (!(funcA = Function::parse(&obj1))) {
        goto err4;
    }
    if (funcA->getInputSize() != 1) {
        error(errSyntaxWarning, -1, "Bad SeparationColorSpace function");
        goto err5;
    }
    cs = new GfxSeparationColorSpace(nameA, altA, funcA);
    return cs;

err5:
    delete funcA;
err4:
    delete altA;
err3:
    delete nameA;
err1:
    return nullptr;
}