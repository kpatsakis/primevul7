void GfxCalRGBColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
    GfxRGB rgb;

#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_GRAY) {
        unsigned char out[gfxColorMaxComps];
        double in[gfxColorMaxComps];
        double X, Y, Z;

        getXYZ(color, &X, &Y, &Z);
        in[0] = clip01(X);
        in[1] = clip01(Y);
        in[2] = clip01(Z);
        transform->doTransform(in, out, 1);
        *gray = byteToCol(out[0]);
        return;
    }
#endif
    getRGB(color, &rgb);
    *gray = clip01((GfxColorComp)(0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b + 0.5));
}