cmsToneCurve* _cmsBuildKToneCurve(cmsContext        ContextID,
                                   cmsUInt32Number   nPoints,
                                   cmsUInt32Number   nProfiles,
                                   const cmsUInt32Number Intents[],
                                   const cmsHPROFILE hProfiles[],
                                   const cmsBool     BPC[],
                                   const cmsFloat64Number AdaptationStates[],
                                   cmsUInt32Number   dwFlags)
{
    cmsToneCurve *in, *out, *KTone;

    // Make sure CMYK -> CMYK
    if (cmsGetColorSpace(hProfiles[0]) != cmsSigCmykData ||
        cmsGetColorSpace(hProfiles[nProfiles-1])!= cmsSigCmykData) return NULL;


    // Make sure last is an output profile
    if (cmsGetDeviceClass(hProfiles[nProfiles - 1]) != cmsSigOutputClass) return NULL;

    // Create individual curves. BPC works also as each K to L* is
    // computed as a BPC to zero black point in case of L*
    in  = ComputeKToLstar(ContextID, nPoints, nProfiles - 1, Intents, hProfiles, BPC, AdaptationStates, dwFlags);
    if (in == NULL) return NULL;

    out = ComputeKToLstar(ContextID, nPoints, 1,
                            Intents + (nProfiles - 1),
                            hProfiles + (nProfiles - 1),
                            BPC + (nProfiles - 1),
                            AdaptationStates + (nProfiles - 1),
                            dwFlags);
    if (out == NULL) {
        cmsFreeToneCurve(in);
        return NULL;
    }

    // Build the relationship. This effectively limits the maximum accuracy to 16 bits, but
    // since this is used on black-preserving LUTs, we are not loosing  accuracy in any case
    KTone = cmsJoinToneCurve(ContextID, in, out, nPoints);

    // Get rid of components
    cmsFreeToneCurve(in); cmsFreeToneCurve(out);

    // Something went wrong...
    if (KTone == NULL) return NULL;

    // Make sure it is monotonic
    if (!cmsIsToneCurveMonotonic(KTone)) {
        cmsFreeToneCurve(KTone);
        return NULL;
    }

    return KTone;
}