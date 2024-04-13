void GfxDeviceRGBColorSpace::getRGBLine(unsigned char *in, unsigned char *out, int length)
{
    for (int i = 0; i < length; i++) {
        *out++ = *in++;
        *out++ = *in++;
        *out++ = *in++;
    }
}