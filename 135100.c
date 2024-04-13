GfxColorSpace *GfxICCBasedColorSpace::parse(Array *arr, OutputDev *out, GfxState *state, int recursion)
{
    GfxICCBasedColorSpace *cs;
    int nCompsA;
    GfxColorSpace *altA;
    Dict *dict;
    Object obj1, obj2;
    int i;

    if (arr->getLength() < 2) {
        error(errSyntaxError, -1, "Bad ICCBased color space");
        return nullptr;
    }
    const Object &obj1Ref = arr->getNF(1);
    const Ref iccProfileStreamA = obj1Ref.isRef() ? obj1Ref.getRef() : Ref::INVALID();
#ifdef USE_CMS
    // check cache
    if (out && iccProfileStreamA != Ref::INVALID()) {
        if (auto *item = out->getIccColorSpaceCache()->lookup(iccProfileStreamA)) {
            cs = static_cast<GfxICCBasedColorSpace *>(item->copy());
            int transformIntent = cs->getIntent();
            int cmsIntent = INTENT_RELATIVE_COLORIMETRIC;
            if (state != nullptr) {
                cmsIntent = state->getCmsRenderingIntent();
            }
            if (transformIntent == cmsIntent) {
                return cs;
            }
            delete cs;
        }
    }
#endif
    obj1 = arr->get(1);
    if (!obj1.isStream()) {
        error(errSyntaxWarning, -1, "Bad ICCBased color space (stream)");
        return nullptr;
    }
    dict = obj1.streamGetDict();
    obj2 = dict->lookup("N");
    if (!obj2.isInt()) {
        error(errSyntaxWarning, -1, "Bad ICCBased color space (N)");
        return nullptr;
    }
    nCompsA = obj2.getInt();
    if (nCompsA > 4) {
        error(errSyntaxError, -1, "ICCBased color space with too many ({0:d} > 4) components", nCompsA);
        nCompsA = 4;
    }
    obj2 = dict->lookup("Alternate");
    if (obj2.isNull() || !(altA = GfxColorSpace::parse(nullptr, &obj2, out, state, recursion + 1))) {
        switch (nCompsA) {
        case 1:
            altA = new GfxDeviceGrayColorSpace();
            break;
        case 3:
            altA = new GfxDeviceRGBColorSpace();
            break;
        case 4:
            altA = new GfxDeviceCMYKColorSpace();
            break;
        default:
            error(errSyntaxWarning, -1, "Bad ICCBased color space - invalid N");
            return nullptr;
        }
    }
    if (altA->getNComps() != nCompsA) {
        error(errSyntaxWarning, -1, "Bad ICCBased color space - N doesn't match alt color space");
        delete altA;
        return nullptr;
    }
    cs = new GfxICCBasedColorSpace(nCompsA, altA, &iccProfileStreamA);
    obj2 = dict->lookup("Range");
    if (obj2.isArray() && obj2.arrayGetLength() == 2 * nCompsA) {
        for (i = 0; i < nCompsA; ++i) {
            cs->rangeMin[i] = obj2.arrayGet(2 * i).getNumWithDefaultValue(0);
            cs->rangeMax[i] = obj2.arrayGet(2 * i + 1).getNumWithDefaultValue(1);
        }
    }

#ifdef USE_CMS
    obj1 = arr->get(1);
    unsigned char *profBuf;
    Stream *iccStream = obj1.getStream();
    int length = 0;

    profBuf = iccStream->toUnsignedChars(&length, 65536, 65536);
    auto hp = make_GfxLCMSProfilePtr(cmsOpenProfileFromMem(profBuf, length));
    cs->profile = hp;
    gfree(profBuf);
    if (!hp) {
        error(errSyntaxWarning, -1, "read ICCBased color space profile error");
    } else {
        auto dhp = (state != nullptr && state->getDisplayProfile() != nullptr) ? state->getDisplayProfile() : nullptr;
        if (!dhp) {
            dhp = GfxState::sRGBProfile;
        }
        unsigned int cst = getCMSColorSpaceType(cmsGetColorSpace(hp.get()));
        unsigned int dNChannels = getCMSNChannels(cmsGetColorSpace(dhp.get()));
        unsigned int dcst = getCMSColorSpaceType(cmsGetColorSpace(dhp.get()));
        cmsHTRANSFORM transform;

        int cmsIntent = INTENT_RELATIVE_COLORIMETRIC;
        if (state != nullptr) {
            cmsIntent = state->getCmsRenderingIntent();
        }
        if ((transform = cmsCreateTransform(hp.get(), COLORSPACE_SH(cst) | CHANNELS_SH(nCompsA) | BYTES_SH(1), dhp.get(), COLORSPACE_SH(dcst) | CHANNELS_SH(dNChannels) | BYTES_SH(1), cmsIntent, LCMS_FLAGS)) == nullptr) {
            error(errSyntaxWarning, -1, "Can't create transform");
            cs->transform = nullptr;
        } else {
            cs->transform = std::make_shared<GfxColorTransform>(transform, cmsIntent, cst, dcst);
        }
        if (dcst == PT_RGB || dcst == PT_CMYK) {
            // create line transform only when the display is RGB type color space
            if ((transform = cmsCreateTransform(hp.get(), CHANNELS_SH(nCompsA) | BYTES_SH(1), dhp.get(), (dcst == PT_RGB) ? TYPE_RGB_8 : TYPE_CMYK_8, cmsIntent, LCMS_FLAGS)) == nullptr) {
                error(errSyntaxWarning, -1, "Can't create transform");
                cs->lineTransform = nullptr;
            } else {
                cs->lineTransform = std::make_shared<GfxColorTransform>(transform, cmsIntent, cst, dcst);
            }
        }
    }
    // put this colorSpace into cache
    if (out && iccProfileStreamA != Ref::INVALID()) {
        out->getIccColorSpaceCache()->put(iccProfileStreamA, static_cast<GfxICCBasedColorSpace *>(cs->copy()));
    }
#endif
    return cs;
}