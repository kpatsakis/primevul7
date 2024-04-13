cmsFloat64Number CMSEXPORT cmsDetectTAC(cmsHPROFILE hProfile)
{
    cmsTACestimator bp;
    cmsUInt32Number dwFormatter;
    cmsUInt32Number GridPoints[MAX_INPUT_DIMENSIONS];
    cmsHPROFILE hLab;
    cmsContext ContextID = cmsGetProfileContextID(hProfile);

    // TAC only works on output profiles
    if (cmsGetDeviceClass(hProfile) != cmsSigOutputClass) {
        return 0;
    }

    // Create a fake formatter for result
    dwFormatter = cmsFormatterForColorspaceOfProfile(hProfile, 4, TRUE);

    bp.nOutputChans = T_CHANNELS(dwFormatter);
    bp.MaxTAC = 0;    // Initial TAC is 0

    //  for safety
    if (bp.nOutputChans >= cmsMAXCHANNELS) return 0;

    hLab = cmsCreateLab4ProfileTHR(ContextID, NULL);
    if (hLab == NULL) return 0;
    // Setup a roundtrip on perceptual intent in output profile for TAC estimation
    bp.hRoundTrip = cmsCreateTransformTHR(ContextID, hLab, TYPE_Lab_16,
                                          hProfile, dwFormatter, INTENT_PERCEPTUAL, cmsFLAGS_NOOPTIMIZE|cmsFLAGS_NOCACHE);

    cmsCloseProfile(hLab);
    if (bp.hRoundTrip == NULL) return 0;

    // For L* we only need black and white. For C* we need many points
    GridPoints[0] = 6;
    GridPoints[1] = 74;
    GridPoints[2] = 74;


	if (!cmsSliceSpace16(3, GridPoints, EstimateTAC, &bp)) {
		bp.MaxTAC = 0;
	}

    cmsDeleteTransform(bp.hRoundTrip);

    // Results in %
    return bp.MaxTAC;
}