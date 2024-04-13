GfxDeviceNColorSpace::GfxDeviceNColorSpace(int nCompsA, std::vector<std::string> &&namesA, GfxColorSpace *altA, Function *funcA, std::vector<GfxSeparationColorSpace *> *sepsCSA) : nComps(nCompsA), names(std::move(namesA))
{
    alt = altA;
    func = funcA;
    sepsCS = sepsCSA;
    nonMarking = true;
    overprintMask = 0;
    mapping = nullptr;
    for (int i = 0; i < nComps; ++i) {
        if (names[i] != "None") {
            nonMarking = false;
        }
        if (names[i] == "Cyan") {
            overprintMask |= 0x01;
        } else if (names[i] == "Magenta") {
            overprintMask |= 0x02;
        } else if (names[i] == "Yellow") {
            overprintMask |= 0x04;
        } else if (names[i] == "Black") {
            overprintMask |= 0x08;
        } else if (names[i] == "All") {
            overprintMask = 0xffffffff;
        } else {
            overprintMask = 0x0f;
        }
    }
}