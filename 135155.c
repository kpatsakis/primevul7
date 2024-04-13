GfxColorSpace *GfxLabColorSpace::copy() const
{
    GfxLabColorSpace *cs;

    cs = new GfxLabColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->aMin = aMin;
    cs->aMax = aMax;
    cs->bMin = bMin;
    cs->bMax = bMax;
    cs->kr = kr;
    cs->kg = kg;
    cs->kb = kb;
#ifdef USE_CMS
    cs->transform = transform;
#endif
    return cs;
}