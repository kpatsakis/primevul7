void GfxDeviceRGBColorSpace::getRGBLine(unsigned char *in, unsigned int *out, int length)
{
    unsigned char *p;
    int i;

    for (i = 0, p = in; i < length; i++, p += 3)
        out[i] = (p[0] << 16) | (p[1] << 8) | (p[2] << 0);
}