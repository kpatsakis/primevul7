void GfxSeparationColorSpace::createMapping(std::vector<GfxSeparationColorSpace *> *separationList, int maxSepComps)
{
    if (nonMarking)
        return;
    mapping = (int *)gmalloc(sizeof(int));
    switch (overprintMask) {
    case 0x01:
        *mapping = 0;
        break;
    case 0x02:
        *mapping = 1;
        break;
    case 0x04:
        *mapping = 2;
        break;
    case 0x08:
        *mapping = 3;
        break;
    default:
        unsigned int newOverprintMask = 0x10;
        for (std::size_t i = 0; i < separationList->size(); i++) {
            GfxSeparationColorSpace *sepCS = (*separationList)[i];
            if (!sepCS->getName()->cmp(name)) {
                if (sepCS->getFunc()->hasDifferentResultSet(func)) {
                    error(errSyntaxWarning, -1, "Different functions found for '{0:t}', convert immediately", name);
                    gfree(mapping);
                    mapping = nullptr;
                    return;
                }
                *mapping = i + 4;
                overprintMask = newOverprintMask;
                return;
            }
            newOverprintMask <<= 1;
        }
        if ((int)separationList->size() == maxSepComps) {
            error(errSyntaxWarning, -1, "Too many ({0:d}) spots, convert '{1:t}' immediately", maxSepComps, name);
            gfree(mapping);
            mapping = nullptr;
            return;
        }
        *mapping = separationList->size() + 4;
        separationList->push_back((GfxSeparationColorSpace *)copy());
        overprintMask = newOverprintMask;
        break;
    }
}