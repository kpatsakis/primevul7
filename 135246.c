void GfxICCBasedColorSpace::getCMYKLine(unsigned char *in, unsigned char *out, int length)
{
#ifdef USE_CMS
    if (lineTransform != nullptr && lineTransform->getTransformPixelType() == PT_CMYK) {
        transform->doTransform(in, out, length);
    } else if (lineTransform != nullptr && nComps != 4) {
        GfxColorComp c, m, y, k;
        unsigned char *tmp = (unsigned char *)gmallocn(3 * length, sizeof(unsigned char));
        getRGBLine(in, tmp, length);
        unsigned char *p = tmp;
        for (int i = 0; i < length; i++) {
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
            *out++ = colToByte(c - k);
            *out++ = colToByte(m - k);
            *out++ = colToByte(y - k);
            *out++ = colToByte(k);
        }
        gfree(tmp);
    } else {
        alt->getCMYKLine(in, out, length);
    }
#else
    alt->getCMYKLine(in, out, length);
#endif
}