void GfxDeviceNColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    clearGfxColor(deviceN);
    if (mapping == nullptr) {
        GfxCMYK cmyk;

        getCMYK(color, &cmyk);
        deviceN->c[0] = cmyk.c;
        deviceN->c[1] = cmyk.m;
        deviceN->c[2] = cmyk.y;
        deviceN->c[3] = cmyk.k;
    } else {
        for (int j = 0; j < nComps; j++)
            if (mapping[j] != -1)
                deviceN->c[mapping[j]] = color->c[j];
    }
}