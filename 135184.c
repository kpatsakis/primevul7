void GfxICCBasedColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_RGB) {
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
                rgb->r = byteToCol(value >> 16);
                rgb->g = byteToCol((value >> 8) & 0xff);
                rgb->b = byteToCol(value & 0xff);
                return;
            }
        }
        transform->doTransform(in, out, 1);
        rgb->r = byteToCol(out[0]);
        rgb->g = byteToCol(out[1]);
        rgb->b = byteToCol(out[2]);
        if (nComps <= 4 && cmsCache.size() <= CMSCACHE_LIMIT) {
            unsigned int key = 0;
            for (int j = 0; j < nComps; j++) {
                key = (key << 8) + in[j];
            }
            unsigned int value = (out[0] << 16) + (out[1] << 8) + out[2];
            cmsCache.insert(std::pair<unsigned int, unsigned int>(key, value));
        }
    } else if (transform != nullptr && transform->getTransformPixelType() == PT_CMYK) {
        unsigned char in[gfxColorMaxComps];
        unsigned char out[gfxColorMaxComps];
        double c, m, y, k, c1, m1, y1, k1, r, g, b;

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
                rgb->r = byteToCol(value >> 16);
                rgb->g = byteToCol((value >> 8) & 0xff);
                rgb->b = byteToCol(value & 0xff);
                return;
            }
        }
        transform->doTransform(in, out, 1);
        c = byteToDbl(out[0]);
        m = byteToDbl(out[1]);
        y = byteToDbl(out[2]);
        k = byteToDbl(out[3]);
        c1 = 1 - c;
        m1 = 1 - m;
        y1 = 1 - y;
        k1 = 1 - k;
        cmykToRGBMatrixMultiplication(c, m, y, k, c1, m1, y1, k1, r, g, b);
        rgb->r = clip01(dblToCol(r));
        rgb->g = clip01(dblToCol(g));
        rgb->b = clip01(dblToCol(b));
        if (nComps <= 4 && cmsCache.size() <= CMSCACHE_LIMIT) {
            unsigned int key = 0;
            for (int j = 0; j < nComps; j++) {
                key = (key << 8) + in[j];
            }
            unsigned int value = (dblToByte(r) << 16) + (dblToByte(g) << 8) + dblToByte(b);
            cmsCache.insert(std::pair<unsigned int, unsigned int>(key, value));
        }
    } else {
        alt->getRGB(color, rgb);
    }
#else
    alt->getRGB(color, rgb);
#endif
}