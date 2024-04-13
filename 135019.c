void GfxDeviceCMYKColorSpace::getRGBLine(unsigned char *in, unsigned char *out, int length)
{
    double r, g, b;

    for (int i = 0; i < length; i++) {
        GfxDeviceCMYKColorSpacegetRGBLineHelper(in, r, g, b);
        *out++ = dblToByte(clip01(r));
        *out++ = dblToByte(clip01(g));
        *out++ = dblToByte(clip01(b));
    }
}