void GfxICCBasedColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_CMYK) {
        unsigned char in[gfxColorMaxComps];
        unsigned char out[gfxColorMaxComps];

        if (nComps == 3 && transform->getInputPixelType() == PT_Lab) {
            in[0] = colToByte(dblToCol(colToDbl(color->c[0]) / 100.0));
            in[1] = colToByte(dblToCol((colToDbl(color->c[1]) + 128.0) / 255.0));
            in[2] = colToByte(dblToCol((colToDbl(color->c[2]) + 128.0) / 255.0));
        } else {
            for (int i = 0; i < nComps; i++) {
                in[i] = colToByte(color->c[i]);
            }
        }
        if (nComps <= 4) {
            unsigned int key = 0;
            for (int j = 0; j < nComps; j++) {
                key = (key << 8) + in[j];
            }
            std::map<unsigned int, unsigned int>::iterator it = cmsCache.find(key);
            if (it != cmsCache.end()) {
                unsigned int value = it->second;
                cmyk->c = byteToCol(value >> 24);
                cmyk->m = byteToCol((value >> 16) & 0xff);
                cmyk->y = byteToCol((value >> 8) & 0xff);
                cmyk->k = byteToCol(value & 0xff);
                return;
            }
        }
        transform->doTransform(in, out, 1);
        cmyk->c = byteToCol(out[0]);
        cmyk->m = byteToCol(out[1]);
        cmyk->y = byteToCol(out[2]);
        cmyk->k = byteToCol(out[3]);
        if (nComps <= 4 && cmsCache.size() <= CMSCACHE_LIMIT) {
            unsigned int key = 0;
            for (int j = 0; j < nComps; j++) {
                key = (key << 8) + in[j];
            }
            unsigned int value = (out[0] << 24) + (out[1] << 16) + (out[2] << 8) + out[3];
            cmsCache.insert(std::pair<unsigned int, unsigned int>(key, value));
        }
    } else if (nComps != 4 && transform != nullptr && transform->getTransformPixelType() == PT_RGB) {
        GfxRGB rgb;
        GfxColorComp c, m, y, k;

        getRGB(color, &rgb);
        c = clip01(gfxColorComp1 - rgb.r);
        m = clip01(gfxColorComp1 - rgb.g);
        y = clip01(gfxColorComp1 - rgb.b);
        k = c;
        if (m < k) {
            k = m;
        }
        if (y < k) {
            k = y;
        }
        cmyk->c = c - k;
        cmyk->m = m - k;
        cmyk->y = y - k;
        cmyk->k = k;
    } else {
        alt->getCMYK(color, cmyk);
    }
#else
    alt->getCMYK(color, cmyk);
#endif
}