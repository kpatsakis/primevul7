void GfxDeviceRGBColorSpace::getDeviceNLine(unsigned char *in, unsigned char *out, int length)
{
    GfxColorComp c, m, y, k;

    for (int i = 0; i < length; i++) {
        for (int j = 0; j < SPOT_NCOMPS + 4; j++)
            out[j] = 0;
        c = byteToCol(255 - *in++);
        m = byteToCol(255 - *in++);
        y = byteToCol(255 - *in++);
        k = c;
        if (m < k) {
            k = m;
        }
        if (y < k) {
            k = y;
        }
        out[0] = colToByte(c - k);
        out[1] = colToByte(m - k);
        out[2] = colToByte(y - k);
        out[3] = colToByte(k);
        out += (SPOT_NCOMPS + 4);
    }
}