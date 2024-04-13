void GfxCalRGBColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    double X, Y, Z;
    double r, g, b;

    getXYZ(color, &X, &Y, &Z);
#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_RGB) {
        unsigned char out[gfxColorMaxComps];
        double in[gfxColorMaxComps];

        in[0] = clip01(X / whiteX);
        in[1] = clip01(Y / whiteY);
        in[2] = clip01(Z / whiteZ);
        transform->doTransform(in, out, 1);
        rgb->r = byteToCol(out[0]);
        rgb->g = byteToCol(out[1]);
        rgb->b = byteToCol(out[2]);
        return;
    }
#endif
    // convert XYZ to RGB, including gamut mapping and gamma correction
    r = xyzrgb[0][0] * X + xyzrgb[0][1] * Y + xyzrgb[0][2] * Z;
    g = xyzrgb[1][0] * X + xyzrgb[1][1] * Y + xyzrgb[1][2] * Z;
    b = xyzrgb[2][0] * X + xyzrgb[2][1] * Y + xyzrgb[2][2] * Z;
    rgb->r = dblToCol(sqrt(clip01(r)));
    rgb->g = dblToCol(sqrt(clip01(g)));
    rgb->b = dblToCol(sqrt(clip01(b)));
}