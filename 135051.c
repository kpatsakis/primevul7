void GfxDeviceCMYKColorSpace::getRGBLine(unsigned char *in, unsigned int *out, int length)
{
    double r, g, b;
    for (int i = 0; i < length; i++) {
        GfxDeviceCMYKColorSpacegetRGBLineHelper(in, r, g, b);
        *out++ = (dblToByte(clip01(r)) << 16) | (dblToByte(clip01(g)) << 8) | dblToByte(clip01(b));
    }
}