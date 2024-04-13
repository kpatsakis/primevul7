GfxSeparationColorSpace::GfxSeparationColorSpace(GooString *nameA, GfxColorSpace *altA, Function *funcA, bool nonMarkingA, unsigned int overprintMaskA, int *mappingA)
{
    name = nameA;
    alt = altA;
    func = funcA;
    nonMarking = nonMarkingA;
    overprintMask = overprintMaskA;
    mapping = mappingA;
}