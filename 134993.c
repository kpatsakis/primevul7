GfxColorSpace *GfxIndexedColorSpace::copy() const
{
    GfxIndexedColorSpace *cs;

    cs = new GfxIndexedColorSpace(base->copy(), indexHigh);
    memcpy(cs->lookup, lookup, (indexHigh + 1) * base->getNComps() * sizeof(unsigned char));
    return cs;
}