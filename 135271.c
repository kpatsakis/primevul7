char *GfxICCBasedColorSpace::getPostScriptCSA()
{
#    if LCMS_VERSION >= 2070
    // The runtime version check of lcms2 is only available from release 2.7 upwards.
    // The generation of the CSA code only works reliably for version 2.10 and upwards.
    // Cf. the explanation in the corresponding lcms2 merge request [1], and the original mail thread [2].
    // [1] https://github.com/mm2/Little-CMS/pull/214
    // [2] https://sourceforge.net/p/lcms/mailman/message/33182987/
    if (cmsGetEncodedCMMversion() < 2100)
        return nullptr;

    int size;

    if (psCSA)
        return psCSA;

    if (!profile) {
        error(errSyntaxWarning, -1, "profile is nullptr");
        return nullptr;
    }

    void *rawprofile = profile.get();
    size = cmsGetPostScriptCSA(cmsGetProfileContextID(rawprofile), rawprofile, getIntent(), 0, nullptr, 0);
    if (size == 0) {
        error(errSyntaxWarning, -1, "PostScript CSA is nullptr");
        return nullptr;
    }

    psCSA = (char *)gmalloc(size + 1);
    cmsGetPostScriptCSA(cmsGetProfileContextID(rawprofile), rawprofile, getIntent(), 0, psCSA, size);
    psCSA[size] = 0;

    // TODO REMOVE-ME-IN-THE-FUTURE
    // until we can depend on https://github.com/mm2/Little-CMS/issues/223 being fixed
    // lcms returns ps code with , instead of . for some locales. The lcms author says
    // that there's no room for any , in the rest of the ps code, so replacing all the , with .
    // is an "acceptable" workaround
    for (int i = 0; i < size; ++i) {
        if (psCSA[i] == ',')
            psCSA[i] = '.';
    }

    return psCSA;
#    else
    return nullptr;
#    endif
}