void GfxDeviceNColorSpace::createMapping(std::vector<GfxSeparationColorSpace *> *separationList, int maxSepComps)
{
    if (nonMarking) // None
        return;
    mapping = (int *)gmalloc(sizeof(int) * nComps);
    unsigned int newOverprintMask = 0;
    for (int i = 0; i < nComps; i++) {
        if (names[i] == "None") {
            mapping[i] = -1;
        } else if (names[i] == "Cyan") {
            newOverprintMask |= 0x01;
            mapping[i] = 0;
        } else if (names[i] == "Magenta") {
            newOverprintMask |= 0x02;
            mapping[i] = 1;
        } else if (names[i] == "Yellow") {
            newOverprintMask |= 0x04;
            mapping[i] = 2;
        } else if (names[i] == "Black") {
            newOverprintMask |= 0x08;
            mapping[i] = 3;
        } else {
            unsigned int startOverprintMask = 0x10;
            bool found = false;
            const Function *sepFunc = nullptr;
            if (nComps == 1)
                sepFunc = func;
            else {
                for (const GfxSeparationColorSpace *sepCS : *sepsCS) {
                    if (!sepCS->getName()->cmp(names[i])) {
                        sepFunc = sepCS->getFunc();
                        break;
                    }
                }
            }
            for (std::size_t j = 0; j < separationList->size(); j++) {
                GfxSeparationColorSpace *sepCS = (*separationList)[j];
                if (!sepCS->getName()->cmp(names[i])) {
                    if (sepFunc != nullptr && sepCS->getFunc()->hasDifferentResultSet(sepFunc)) {
                        error(errSyntaxWarning, -1, "Different functions found for '{0:s}', convert immediately", names[i].c_str());
                        gfree(mapping);
                        mapping = nullptr;
                        overprintMask = 0xffffffff;
                        return;
                    }
                    mapping[i] = j + 4;
                    newOverprintMask |= startOverprintMask;
                    found = true;
                    break;
                }
                startOverprintMask <<= 1;
            }
            if (!found) {
                if ((int)separationList->size() == maxSepComps) {
                    error(errSyntaxWarning, -1, "Too many ({0:d}) spots, convert '{1:s}' immediately", maxSepComps, names[i].c_str());
                    gfree(mapping);
                    mapping = nullptr;
                    overprintMask = 0xffffffff;
                    return;
                }
                mapping[i] = separationList->size() + 4;
                newOverprintMask |= startOverprintMask;
                if (nComps == 1)
                    separationList->push_back(new GfxSeparationColorSpace(new GooString(names[i]), alt->copy(), func->copy()));
                else {
                    for (const GfxSeparationColorSpace *sepCS : *sepsCS) {
                        if (!sepCS->getName()->cmp(names[i])) {
                            found = true;
                            separationList->push_back((GfxSeparationColorSpace *)sepCS->copy());
                            break;
                        }
                    }
                    if (!found) {
                        error(errSyntaxWarning, -1, "DeviceN has no suitable colorant");
                        gfree(mapping);
                        mapping = nullptr;
                        overprintMask = 0xffffffff;
                        return;
                    }
                }
            }
        }
    }
    overprintMask = newOverprintMask;
}