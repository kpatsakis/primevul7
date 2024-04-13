void GfxImageColorMap::getDeviceNLine(unsigned char *in, unsigned char *out, int length)
{
    unsigned char *inp, *tmp_line;

    if (!useDeviceNLine()) {
        GfxColor deviceN;

        inp = in;
        for (int i = 0; i < length; i++) {
            getDeviceN(inp, &deviceN);
            for (int j = 0; j < SPOT_NCOMPS + 4; j++)
                *out++ = deviceN.c[j];
            inp += nComps;
        }
        return;
    }

    switch (colorSpace->getMode()) {
    case csIndexed:
    case csSeparation:
        tmp_line = (unsigned char *)gmallocn(length, nComps2);
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < nComps2; j++) {
                unsigned char c = in[i];
                if (byte_lookup)
                    c = byte_lookup[c * nComps2 + j];
                tmp_line[i * nComps2 + j] = c;
            }
        }
        colorSpace2->getDeviceNLine(tmp_line, out, length);
        gfree(tmp_line);
        break;

    default:
        if (byte_lookup) {
            inp = in;
            for (int j = 0; j < length; j++)
                for (int i = 0; i < nComps; i++) {
                    *inp = byte_lookup[*inp * nComps + i];
                    inp++;
                }
        }
        colorSpace->getDeviceNLine(in, out, length);
        break;
    }
}