void GfxICCBasedColorSpace::getRGBLine(unsigned char *in, unsigned char *out, int length)
{
#ifdef USE_CMS
    if (lineTransform != nullptr && lineTransform->getTransformPixelType() == PT_RGB) {
        unsigned char *tmp = (unsigned char *)gmallocn(3 * length, sizeof(unsigned char));
        lineTransform->doTransform(in, tmp, length);
        unsigned char *current = tmp;
        for (int i = 0; i < length; ++i) {
            *out++ = *current++;
            *out++ = *current++;
            *out++ = *current++;
        }
        gfree(tmp);
    } else if (lineTransform != nullptr && lineTransform->getTransformPixelType() == PT_CMYK) {
        unsigned char *tmp = (unsigned char *)gmallocn(4 * length, sizeof(unsigned char));
        lineTransform->doTransform(in, tmp, length);
        unsigned char *current = tmp;
        double c, m, y, k, c1, m1, y1, k1, r, g, b;
        for (int i = 0; i < length; ++i) {
            c = byteToDbl(*current++);
            m = byteToDbl(*current++);
            y = byteToDbl(*current++);
            k = byteToDbl(*current++);
            c1 = 1 - c;
            m1 = 1 - m;
            y1 = 1 - y;
            k1 = 1 - k;
            cmykToRGBMatrixMultiplication(c, m, y, k, c1, m1, y1, k1, r, g, b);
            *out++ = dblToByte(r);
            *out++ = dblToByte(g);
            *out++ = dblToByte(b);
        }
        gfree(tmp);
    } else {
        alt->getRGBLine(in, out, length);
    }
#else
    alt->getRGBLine(in, out, length);
#endif
}