void GfxSeparationColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    clearGfxColor(deviceN);
    if (mapping == nullptr || mapping[0] == -1) {
        GfxCMYK cmyk;

        getCMYK(color, &cmyk);
        deviceN->c[0] = cmyk.c;
        deviceN->c[1] = cmyk.m;
        deviceN->c[2] = cmyk.y;
        deviceN->c[3] = cmyk.k;
    } else {
        deviceN->c[mapping[0]] = color->c[0];
    }
}