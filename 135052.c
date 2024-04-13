GfxIndexedColorSpace::GfxIndexedColorSpace(GfxColorSpace *baseA, int indexHighA)
{
    base = baseA;
    indexHigh = indexHighA;
    lookup = (unsigned char *)gmallocn((indexHigh + 1) * base->getNComps(), sizeof(unsigned char));
    overprintMask = base->getOverprintMask();
}