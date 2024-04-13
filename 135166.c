void GfxImageColorMap::getRGBLine(unsigned char *in, unsigned int *out, int length)
{
    int i, j;
    unsigned char *inp, *tmp_line;

    if (!useRGBLine()) {
        GfxRGB rgb;

        inp = in;
        for (i = 0; i < length; i++) {
            getRGB(inp, &rgb);
            out[i] = ((int)colToByte(rgb.r) << 16) | ((int)colToByte(rgb.g) << 8) | ((int)colToByte(rgb.b) << 0);
            inp += nComps;
        }
        return;
    }

    switch (colorSpace->getMode()) {
    case csIndexed:
    case csSeparation:
        tmp_line = (unsigned char *)gmallocn(length, nComps2);
        for (i = 0; i < length; i++) {
            for (j = 0; j < nComps2; j++) {
                unsigned char c = in[i];
                if (byte_lookup)
                    c = byte_lookup[c * nComps2 + j];
                tmp_line[i * nComps2 + j] = c;
            }
        }
        colorSpace2->getRGBLine(tmp_line, out, length);
        gfree(tmp_line);
        break;

    default:
        if (byte_lookup) {
            inp = in;
            for (j = 0; j < length; j++)
                for (i = 0; i < nComps; i++) {
                    *inp = byte_lookup[*inp * nComps + i];
                    inp++;
                }
        }
        colorSpace->getRGBLine(in, out, length);
        break;
    }
}