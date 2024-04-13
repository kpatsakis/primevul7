void GfxICCBasedColorSpace::getRGBLine(unsigned char *in, unsigned int *out, int length)
{
#ifdef USE_CMS
    if (lineTransform != nullptr && lineTransform->getTransformPixelType() == PT_RGB) {
        unsigned char *tmp = (unsigned char *)gmallocn(3 * length, sizeof(unsigned char));
        lineTransform->doTransform(in, tmp, length);
        for (int i = 0; i < length; ++i) {
            unsigned char *current = tmp + (i * 3);
            out[i] = (current[0] << 16) | (current[1] << 8) | current[2];
        }
        gfree(tmp);
    } else {
        alt->getRGBLine(in, out, length);
    }
#else
    alt->getRGBLine(in, out, length);
#endif
}