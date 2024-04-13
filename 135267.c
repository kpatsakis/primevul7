void GfxLabColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    double X, Y, Z;

    getXYZ(color, &X, &Y, &Z);
#ifdef USE_CMS
    if (transform != nullptr && transform->getTransformPixelType() == PT_RGB) {
        unsigned char out[gfxColorMaxComps];
        double in[gfxColorMaxComps];

        in[0] = clip01(X);
        in[1] = clip01(Y);
        in[2] = clip01(Z);
        transform->doTransform(in, out, 1);
        rgb->r = byteToCol(out[0]);
        rgb->g = byteToCol(out[1]);
        rgb->b = byteToCol(out[2]);
        return;
    } else if (transform != nullptr && transform->getTransformPixelType() == PT_CMYK) {
        unsigned char out[gfxColorMaxComps];
        double in[gfxColorMaxComps];
        double c, m, y, k, c1, m1, y1, k1, r, g, b;

        in[0] = clip01(X);
        in[1] = clip01(Y);
        in[2] = clip01(Z);
        transform->doTransform(in, out, 1);
        c = byteToDbl(out[0]);
        m = byteToDbl(out[1]);
        y = byteToDbl(out[2]);
        k = byteToDbl(out[3]);
        c1 = 1 - c;
        m1 = 1 - m;
        y1 = 1 - y;
        k1 = 1 - k;
        cmykToRGBMatrixMultiplication(c, m, y, k, c1, m1, y1, k1, r, g, b);
        rgb->r = clip01(dblToCol(r));
        rgb->g = clip01(dblToCol(g));
        rgb->b = clip01(dblToCol(b));
        return;
    }
#endif
    X *= whiteX;
    Y *= whiteY;
    Z *= whiteZ;
    // convert XYZ to RGB, including gamut mapping and gamma correction
    const double r = xyzrgb[0][0] * X + xyzrgb[0][1] * Y + xyzrgb[0][2] * Z;
    const double g = xyzrgb[1][0] * X + xyzrgb[1][1] * Y + xyzrgb[1][2] * Z;
    const double b = xyzrgb[2][0] * X + xyzrgb[2][1] * Y + xyzrgb[2][2] * Z;
    rgb->r = dblToCol(sqrt(clip01(r * kr)));
    rgb->g = dblToCol(sqrt(clip01(g * kg)));
    rgb->b = dblToCol(sqrt(clip01(b * kb)));
}