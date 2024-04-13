void GfxICCBasedColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_GRAY) {
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
                *gray = byteToCol(value & 0xff);
                return;
            }
        }
        transform->doTransform(in, out, 1);
        *gray = byteToCol(out[0]);
        if (nComps <= 4 && cmsCache.size() <= CMSCACHE_LIMIT) {
            unsigned int key = 0;
            for (int j = 0; j < nComps; j++) {
                key = (key << 8) + in[j];
            }
            unsigned int value = out[0];
            cmsCache.insert(std::pair<unsigned int, unsigned int>(key, value));
        }
    } else {
        GfxRGB rgb;
        getRGB(color, &rgb);
        *gray = clip01((GfxColorComp)(0.3 * rgb.r + 0.59 * rgb.g + 0.11 * rgb.b + 0.5));
    }
#else
    alt->getGray(color, gray);
#endif
}