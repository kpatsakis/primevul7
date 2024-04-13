void GfxDeviceRGBColorSpace::getCMYKLine(unsigned char *in, unsigned char *out, int length)
{
    GfxColorComp c, m, y, k;

    for (int i = 0; i < length; i++) {
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
        *out++ = colToByte(c - k);
        *out++ = colToByte(m - k);
        *out++ = colToByte(y - k);
        *out++ = colToByte(k);
    }
}