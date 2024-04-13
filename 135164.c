void GfxCalRGBColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    GfxRGB rgb;
    GfxColorComp c, m, y, k;

#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_CMYK) {
        double in[gfxColorMaxComps];
        unsigned char out[gfxColorMaxComps];
        double X, Y, Z;

        getXYZ(color, &X, &Y, &Z);
        in[0] = clip01(X);
        in[1] = clip01(Y);
        in[2] = clip01(Z);
        transform->doTransform(in, out, 1);
        cmyk->c = byteToCol(out[0]);
        cmyk->m = byteToCol(out[1]);
        cmyk->y = byteToCol(out[2]);
        cmyk->k = byteToCol(out[3]);
        return;
    }
#endif
    getRGB(color, &rgb);
    c = clip01(gfxColorComp1 - rgb.r);
    m = clip01(gfxColorComp1 - rgb.g);
    y = clip01(gfxColorComp1 - rgb.b);
    k = c;
    if (m < k) {
        k = m;
    }
    if (y < k) {
        k = y;
    }
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
}