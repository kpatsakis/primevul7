GfxColorSpace *GfxICCBasedColorSpace::copy() const
{
    GfxICCBasedColorSpace *cs;
    int i;

    cs = new GfxICCBasedColorSpace(nComps, alt->copy(), &iccProfileStream);
    for (i = 0; i < 4; ++i) {
        cs->rangeMin[i] = rangeMin[i];
        cs->rangeMax[i] = rangeMax[i];
    }
#ifdef USE_CMS
    cs->profile = profile;
    cs->transform = transform;
    cs->lineTransform = lineTransform;
#endif
    return cs;
}