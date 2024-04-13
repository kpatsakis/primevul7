GfxColorSpace *GfxColorSpace::parse(GfxResources *res, Object *csObj, OutputDev *out, GfxState *state, int recursion)
{
    GfxColorSpace *cs;
    Object obj1;

    if (recursion > colorSpaceRecursionLimit) {
        error(errSyntaxError, -1, "Loop detected in color space objects");
        return nullptr;
    }

    cs = nullptr;
    if (csObj->isName()) {
        if (csObj->isName("DeviceGray") || csObj->isName("G")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultGray");
                if (objCS.isNull()) {
                    cs = new GfxDeviceGrayColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceGrayColorSpace();
            }
        } else if (csObj->isName("DeviceRGB") || csObj->isName("RGB")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultRGB");
                if (objCS.isNull()) {
                    cs = new GfxDeviceRGBColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceRGBColorSpace();
            }
        } else if (csObj->isName("DeviceCMYK") || csObj->isName("CMYK")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultCMYK");
                if (objCS.isNull()) {
                    cs = new GfxDeviceCMYKColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceCMYKColorSpace();
            }
        } else if (csObj->isName("Pattern")) {
            cs = new GfxPatternColorSpace(nullptr);
        } else {
            error(errSyntaxWarning, -1, "Bad color space '{0:s}'", csObj->getName());
        }
    } else if (csObj->isArray() && csObj->arrayGetLength() > 0) {
        obj1 = csObj->arrayGet(0);
        if (obj1.isName("DeviceGray") || obj1.isName("G")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultGray");
                if (objCS.isNull()) {
                    cs = new GfxDeviceGrayColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceGrayColorSpace();
            }
        } else if (obj1.isName("DeviceRGB") || obj1.isName("RGB")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultRGB");
                if (objCS.isNull()) {
                    cs = new GfxDeviceRGBColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceRGBColorSpace();
            }
        } else if (obj1.isName("DeviceCMYK") || obj1.isName("CMYK")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultCMYK");
                if (objCS.isNull()) {
                    cs = new GfxDeviceCMYKColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceCMYKColorSpace();
            }
        } else if (obj1.isName("CalGray")) {
            cs = GfxCalGrayColorSpace::parse(csObj->getArray(), state);
        } else if (obj1.isName("CalRGB")) {
            cs = GfxCalRGBColorSpace::parse(csObj->getArray(), state);
        } else if (obj1.isName("Lab")) {
            cs = GfxLabColorSpace::parse(csObj->getArray(), state);
        } else if (obj1.isName("ICCBased")) {
            cs = GfxICCBasedColorSpace::parse(csObj->getArray(), out, state, recursion);
        } else if (obj1.isName("Indexed") || obj1.isName("I")) {
            cs = GfxIndexedColorSpace::parse(res, csObj->getArray(), out, state, recursion);
        } else if (obj1.isName("Separation")) {
            cs = GfxSeparationColorSpace::parse(res, csObj->getArray(), out, state, recursion);
        } else if (obj1.isName("DeviceN")) {
            cs = GfxDeviceNColorSpace::parse(res, csObj->getArray(), out, state, recursion);
        } else if (obj1.isName("Pattern")) {
            cs = GfxPatternColorSpace::parse(res, csObj->getArray(), out, state, recursion);
        } else {
            error(errSyntaxWarning, -1, "Bad color space");
        }
    } else if (csObj->isDict()) {
        obj1 = csObj->dictLookup("ColorSpace");
        if (obj1.isName("DeviceGray")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultGray");
                if (objCS.isNull()) {
                    cs = new GfxDeviceGrayColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceGrayColorSpace();
            }
        } else if (obj1.isName("DeviceRGB")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultRGB");
                if (objCS.isNull()) {
                    cs = new GfxDeviceRGBColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceRGBColorSpace();
            }
        } else if (obj1.isName("DeviceCMYK")) {
            if (res != nullptr) {
                Object objCS = res->lookupColorSpace("DefaultCMYK");
                if (objCS.isNull()) {
                    cs = new GfxDeviceCMYKColorSpace();
                } else {
                    cs = GfxColorSpace::parse(nullptr, &objCS, out, state);
                }
            } else {
                cs = new GfxDeviceCMYKColorSpace();
            }
        } else {
            error(errSyntaxWarning, -1, "Bad color space dict'");
        }
    } else {
        error(errSyntaxWarning, -1, "Bad color space - expected name or array or dict");
    }
    return cs;
}