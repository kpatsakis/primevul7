GfxColorSpace *GfxCalRGBColorSpace::copy() const
{
    GfxCalRGBColorSpace *cs;
    int i;

    cs = new GfxCalRGBColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->gammaR = gammaR;
    cs->gammaG = gammaG;
    cs->gammaB = gammaB;
    cs->kr = kr;
    cs->kg = kg;
    cs->kb = kb;
    for (i = 0; i < 9; ++i) {
        cs->mat[i] = mat[i];
    }
#ifdef USE_CMS
    cs->transform = transform;
#endif
    return cs;
}