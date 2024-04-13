GfxColorSpace *GfxDeviceNColorSpace::copy() const
{
    int *mappingA = nullptr;

    auto sepsCSA = new std::vector<GfxSeparationColorSpace *>();
    sepsCSA->reserve(sepsCS->size());
    for (const GfxSeparationColorSpace *scs : *sepsCS) {
        if (likely(scs != nullptr)) {
            sepsCSA->push_back((GfxSeparationColorSpace *)scs->copy());
        }
    }
    if (mapping != nullptr) {
        mappingA = (int *)gmalloc(sizeof(int) * nComps);
        for (int i = 0; i < nComps; i++)
            mappingA[i] = mapping[i];
    }
    return new GfxDeviceNColorSpace(nComps, names, alt->copy(), func->copy(), sepsCSA, mappingA, nonMarking, overprintMask);
}