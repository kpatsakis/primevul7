void GfxICCBasedColorSpace::getRGBXLine(unsigned char *in, unsigned char *out, int length)
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
            *out++ = 255;
        }
        gfree(tmp);
    } else {
        alt->getRGBXLine(in, out, length);
    }
#else
    alt->getRGBXLine(in, out, length);
#endif
}