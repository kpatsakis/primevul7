void GfxDeviceGrayColorSpace::getCMYKLine(unsigned char *in, unsigned char *out, int length)
{
    for (int i = 0; i < length; i++) {
        *out++ = 0;
        *out++ = 0;
        *out++ = 0;
        *out++ = in[i];
    }
}