GfxShading *GfxShading::parse(GfxResources *res, Object *obj, OutputDev *out, GfxState *state)
{
    GfxShading *shading;
    Dict *dict;
    int typeA;
    Object obj1;

    if (obj->isDict()) {
        dict = obj->getDict();
    } else if (obj->isStream()) {
        dict = obj->streamGetDict();
    } else {
        return nullptr;
    }

    obj1 = dict->lookup("ShadingType");
    if (!obj1.isInt()) {
        error(errSyntaxWarning, -1, "Invalid ShadingType in shading dictionary");
        return nullptr;
    }
    typeA = obj1.getInt();

    switch (typeA) {
    case 1:
        shading = GfxFunctionShading::parse(res, dict, out, state);
        break;
    case 2:
        shading = GfxAxialShading::parse(res, dict, out, state);
        break;
    case 3:
        shading = GfxRadialShading::parse(res, dict, out, state);
        break;
    case 4:
        if (obj->isStream()) {
            shading = GfxGouraudTriangleShading::parse(res, 4, dict, obj->getStream(), out, state);
        } else {
            error(errSyntaxWarning, -1, "Invalid Type 4 shading object");
            goto err1;
        }
        break;
    case 5:
        if (obj->isStream()) {
            shading = GfxGouraudTriangleShading::parse(res, 5, dict, obj->getStream(), out, state);
        } else {
            error(errSyntaxWarning, -1, "Invalid Type 5 shading object");
            goto err1;
        }
        break;
    case 6:
        if (obj->isStream()) {
            shading = GfxPatchMeshShading::parse(res, 6, dict, obj->getStream(), out, state);
        } else {
            error(errSyntaxWarning, -1, "Invalid Type 6 shading object");
            goto err1;
        }
        break;
    case 7:
        if (obj->isStream()) {
            shading = GfxPatchMeshShading::parse(res, 7, dict, obj->getStream(), out, state);
        } else {
            error(errSyntaxWarning, -1, "Invalid Type 7 shading object");
            goto err1;
        }
        break;
    default:
        error(errSyntaxWarning, -1, "Unimplemented shading type {0:d}", typeA);
        goto err1;
    }

    return shading;

err1:
    return nullptr;
}