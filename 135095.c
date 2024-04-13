GfxICCBasedColorSpace::GfxICCBasedColorSpace(int nCompsA, GfxColorSpace *altA, const Ref *iccProfileStreamA)
{
    nComps = nCompsA;
    alt = altA;
    iccProfileStream = *iccProfileStreamA;
    rangeMin[0] = rangeMin[1] = rangeMin[2] = rangeMin[3] = 0;
    rangeMax[0] = rangeMax[1] = rangeMax[2] = rangeMax[3] = 1;
#ifdef USE_CMS
    transform = nullptr;
    lineTransform = nullptr;
    psCSA = nullptr;
#endif
}