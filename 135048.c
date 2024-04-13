void GfxDeviceGrayColorSpace::getRGBLine(unsigned char *in, unsigned char *out, int length)
{
    for (int i = 0; i < length; i++) {
        *out++ = in[i];
        *out++ = in[i];
        *out++ = in[i];
    }
}