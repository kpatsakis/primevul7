GfxDeviceNColorSpace::GfxDeviceNColorSpace(int nCompsA, const std::vector<std::string> &namesA, GfxColorSpace *altA, Function *funcA, std::vector<GfxSeparationColorSpace *> *sepsCSA, int *mappingA, bool nonMarkingA,
                                           unsigned int overprintMaskA)
    : nComps(nCompsA), names(namesA)
{
    alt = altA;
    func = funcA;
    sepsCS = sepsCSA;
    mapping = mappingA;
    nonMarking = nonMarkingA;
    overprintMask = overprintMaskA;
}