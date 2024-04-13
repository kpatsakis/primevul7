GfxImageColorMap::GfxImageColorMap(int bitsA, Object *decode, GfxColorSpace *colorSpaceA)
{
    GfxIndexedColorSpace *indexedCS;
    GfxSeparationColorSpace *sepCS;
    int maxPixel, indexHigh;
    unsigned char *indexedLookup;
    const Function *sepFunc;
    double x[gfxColorMaxComps];
    double y[gfxColorMaxComps] = {};
    int i, j, k;
    double mapped;
    bool useByteLookup;

    ok = true;
    useMatte = false;

    colorSpace = colorSpaceA;

    // initialize
    for (k = 0; k < gfxColorMaxComps; ++k) {
        lookup[k] = nullptr;
        lookup2[k] = nullptr;
    }
    byte_lookup = nullptr;

    // bits per component and color space
    if (unlikely(bitsA <= 0 || bitsA > 30))
        goto err1;

    bits = bitsA;
    maxPixel = (1 << bits) - 1;

    // this is a hack to support 16 bits images, everywhere
    // we assume a component fits in 8 bits, with this hack
    // we treat 16 bit images as 8 bit ones until it's fixed correctly.
    // The hack has another part on ImageStream::getLine
    if (maxPixel > 255)
        maxPixel = 255;

    // get decode map
    if (decode->isNull()) {
        nComps = colorSpace->getNComps();
        colorSpace->getDefaultRanges(decodeLow, decodeRange, maxPixel);
    } else if (decode->isArray()) {
        nComps = decode->arrayGetLength() / 2;
        if (nComps < colorSpace->getNComps()) {
            goto err1;
        }
        if (nComps > colorSpace->getNComps()) {
            error(errSyntaxWarning, -1, "Too many elements in Decode array");
            nComps = colorSpace->getNComps();
        }
        for (i = 0; i < nComps; ++i) {
            Object obj = decode->arrayGet(2 * i);
            if (!obj.isNum()) {
                goto err1;
            }
            decodeLow[i] = obj.getNum();
            obj = decode->arrayGet(2 * i + 1);
            if (!obj.isNum()) {
                goto err1;
            }
            decodeRange[i] = obj.getNum() - decodeLow[i];
        }
    } else {
        goto err1;
    }

    // Construct a lookup table -- this stores pre-computed decoded
    // values for each component, i.e., the result of applying the
    // decode mapping to each possible image pixel component value.
    for (k = 0; k < nComps; ++k) {
        lookup[k] = (GfxColorComp *)gmallocn(maxPixel + 1, sizeof(GfxColorComp));
        for (i = 0; i <= maxPixel; ++i) {
            lookup[k][i] = dblToCol(decodeLow[k] + (i * decodeRange[k]) / maxPixel);
        }
    }

    // Optimization: for Indexed and Separation color spaces (which have
    // only one component), we pre-compute a second lookup table with
    // color values
    colorSpace2 = nullptr;
    nComps2 = 0;
    useByteLookup = false;
    switch (colorSpace->getMode()) {
    case csIndexed:
        // Note that indexHigh may not be the same as maxPixel --
        // Distiller will remove unused palette entries, resulting in
        // indexHigh < maxPixel.
        indexedCS = (GfxIndexedColorSpace *)colorSpace;
        colorSpace2 = indexedCS->getBase();
        indexHigh = indexedCS->getIndexHigh();
        nComps2 = colorSpace2->getNComps();
        indexedLookup = indexedCS->getLookup();
        colorSpace2->getDefaultRanges(x, y, indexHigh);
        if (colorSpace2->useGetGrayLine() || colorSpace2->useGetRGBLine() || colorSpace2->useGetCMYKLine() || colorSpace2->useGetDeviceNLine()) {
            byte_lookup = (unsigned char *)gmallocn((maxPixel + 1), nComps2);
            useByteLookup = true;
        }
        for (k = 0; k < nComps2; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(maxPixel + 1, sizeof(GfxColorComp));
            for (i = 0; i <= maxPixel; ++i) {
                j = (int)(decodeLow[0] + (i * decodeRange[0]) / maxPixel + 0.5);
                if (j < 0) {
                    j = 0;
                } else if (j > indexHigh) {
                    j = indexHigh;
                }

                mapped = x[k] + (indexedLookup[j * nComps2 + k] / 255.0) * y[k];
                lookup2[k][i] = dblToCol(mapped);
                if (useByteLookup)
                    byte_lookup[i * nComps2 + k] = (unsigned char)(mapped * 255);
            }
        }
        break;
    case csSeparation:
        sepCS = (GfxSeparationColorSpace *)colorSpace;
        colorSpace2 = sepCS->getAlt();
        nComps2 = colorSpace2->getNComps();
        sepFunc = sepCS->getFunc();
        if (colorSpace2->useGetGrayLine() || colorSpace2->useGetRGBLine() || colorSpace2->useGetCMYKLine() || colorSpace2->useGetDeviceNLine()) {
            byte_lookup = (unsigned char *)gmallocn((maxPixel + 1), nComps2);
            useByteLookup = true;
        }
        for (k = 0; k < nComps2; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(maxPixel + 1, sizeof(GfxColorComp));
            for (i = 0; i <= maxPixel; ++i) {
                x[0] = decodeLow[0] + (i * decodeRange[0]) / maxPixel;
                sepFunc->transform(x, y);
                lookup2[k][i] = dblToCol(y[k]);
                if (useByteLookup)
                    byte_lookup[i * nComps2 + k] = (unsigned char)(y[k] * 255);
            }
        }
        break;
    default:
        if ((!decode->isNull() || maxPixel != 255) && (colorSpace->useGetGrayLine() || (colorSpace->useGetRGBLine() && !decode->isNull()) || colorSpace->useGetCMYKLine() || colorSpace->useGetDeviceNLine())) {
            byte_lookup = (unsigned char *)gmallocn((maxPixel + 1), nComps);
            useByteLookup = true;
        }
        for (k = 0; k < nComps; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(maxPixel + 1, sizeof(GfxColorComp));
            for (i = 0; i <= maxPixel; ++i) {
                mapped = decodeLow[k] + (i * decodeRange[k]) / maxPixel;
                lookup2[k][i] = dblToCol(mapped);
                if (useByteLookup) {
                    int byte;

                    byte = (int)(mapped * 255.0 + 0.5);
                    if (byte < 0)
                        byte = 0;
                    else if (byte > 255)
                        byte = 255;
                    byte_lookup[i * nComps + k] = byte;
                }
            }
        }
    }

    return;

err1:
    ok = false;
}