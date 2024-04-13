GfxColorSpace *GfxSeparationColorSpace::copy() const
{
    int *mappingA = nullptr;
    if (mapping != nullptr) {
        mappingA = (int *)gmalloc(sizeof(int));
        *mappingA = *mapping;
    }
    return new GfxSeparationColorSpace(name->copy(), alt->copy(), func->copy(), nonMarking, overprintMask, mappingA);
}