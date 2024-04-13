void GfxICCBasedColorSpace::getDeviceNLine(unsigned char *in, unsigned char *out, int length)
{
#ifdef USE_CMS
    if (lineTransform != nullptr && lineTransform->getTransformPixelType() == PT_CMYK) {
        unsigned char *tmp = (unsigned char *)gmallocn(4 * length, sizeof(unsigned char));
        transform->doTransform(in, tmp, length);
        unsigned char *p = tmp;
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < 4; j++)
                *out++ = *p++;
            for (int j = 4; j < SPOT_NCOMPS + 4; j++)
                *out++ = 0;
        }
        gfree(tmp);
    } else if (lineTransform != nullptr && nComps != 4) {
        GfxColorComp c, m, y, k;
        unsigned char *tmp = (unsigned char *)gmallocn(3 * length, sizeof(unsigned char));
        getRGBLine(in, tmp, length);
        unsigned char *p = tmp;
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < SPOT_NCOMPS + 4; j++)
                out[j] = 0;
            c = byteToCol(255 - *p++);
            m = byteToCol(255 - *p++);
            y = byteToCol(255 - *p++);
            k = c;
            if (m < k) {
                k = m;
            }
            if (y < k) {
                k = y;
            }
            out[0] = colToByte(c - k);
            out[1] = colToByte(m - k);
            out[2] = colToByte(y - k);
            out[3] = colToByte(k);
            out += (SPOT_NCOMPS + 4);
        }
        gfree(tmp);
    } else {
        alt->getDeviceNLine(in, out, length);
    }
#else
    alt->getDeviceNLine(in, out, length);
#endif
}