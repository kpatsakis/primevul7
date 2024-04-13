void GfxDeviceGrayColorSpace::getDeviceNLine(unsigned char *in, unsigned char *out, int length)
{
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < SPOT_NCOMPS + 4; j++)
            out[j] = 0;
        out[4] = in[i];
        out += (SPOT_NCOMPS + 4);
    }
}