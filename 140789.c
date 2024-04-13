cmsHTRANSFORM _cmsChain2Lab(cmsContext            ContextID,
                            cmsUInt32Number        nProfiles,
                            cmsUInt32Number        InputFormat,
                            cmsUInt32Number        OutputFormat,
                            const cmsUInt32Number  Intents[],
                            const cmsHPROFILE      hProfiles[],
                            const cmsBool          BPC[],
                            const cmsFloat64Number AdaptationStates[],
                            cmsUInt32Number        dwFlags)
{
    cmsHTRANSFORM xform;
    cmsHPROFILE   hLab;
    cmsHPROFILE   ProfileList[256];
    cmsBool       BPCList[256];
    cmsFloat64Number AdaptationList[256];
    cmsUInt32Number IntentList[256];
    cmsUInt32Number i;

    // This is a rather big number and there is no need of dynamic memory
    // since we are adding a profile, 254 + 1 = 255 and this is the limit
    if (nProfiles > 254) return NULL;

    // The output space
    hLab = cmsCreateLab4ProfileTHR(ContextID, NULL);
    if (hLab == NULL) return NULL;

    // Create a copy of parameters
    for (i=0; i < nProfiles; i++) {

        ProfileList[i]    = hProfiles[i];
        BPCList[i]        = BPC[i];
        AdaptationList[i] = AdaptationStates[i];
        IntentList[i]     = Intents[i];
    }

    // Place Lab identity at chain's end.
    ProfileList[nProfiles]    = hLab;
    BPCList[nProfiles]        = 0;
    AdaptationList[nProfiles] = 1.0;
    IntentList[nProfiles]     = INTENT_RELATIVE_COLORIMETRIC;

    // Create the transform
    xform = cmsCreateExtendedTransform(ContextID, nProfiles + 1, ProfileList,
                                       BPCList,
                                       IntentList,
                                       AdaptationList,
                                       NULL, 0,
                                       InputFormat,
                                       OutputFormat,
                                       dwFlags);

    cmsCloseProfile(hLab);

    return xform;
}