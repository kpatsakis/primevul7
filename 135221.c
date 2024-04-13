GfxColorSpace *GfxCalGrayColorSpace::copy() const
{
    GfxCalGrayColorSpace *cs;

    cs = new GfxCalGrayColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->gamma = gamma;
    cs->kr = kr;
    cs->kg = kg;
    cs->kb = kb;
#ifdef USE_CMS
    cs->transform = transform;
#endif
    return cs;
}