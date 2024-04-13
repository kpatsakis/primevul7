GfxSeparationColorSpace::GfxSeparationColorSpace(GooString *nameA, GfxColorSpace *altA, Function *funcA)
{
    name = nameA;
    alt = altA;
    func = funcA;
    nonMarking = !name->cmp("None");
    if (!name->cmp("Cyan")) {
        overprintMask = 0x01;
    } else if (!name->cmp("Magenta")) {
        overprintMask = 0x02;
    } else if (!name->cmp("Yellow")) {
        overprintMask = 0x04;
    } else if (!name->cmp("Black")) {
        overprintMask = 0x08;
    } else if (!name->cmp("All")) {
        overprintMask = 0xffffffff;
    }
}