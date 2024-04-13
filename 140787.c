cmsToneCurve* ComputeKToLstar(cmsContext            ContextID,
                               cmsUInt32Number       nPoints,
                               cmsUInt32Number       nProfiles,
                               const cmsUInt32Number Intents[],
                               const cmsHPROFILE     hProfiles[],
                               const cmsBool         BPC[],
                               const cmsFloat64Number AdaptationStates[],
                               cmsUInt32Number dwFlags)
{
    cmsToneCurve* out = NULL;
    cmsUInt32Number i;
    cmsHTRANSFORM xform;
    cmsCIELab Lab;
    cmsFloat32Number cmyk[4];
    cmsFloat32Number* SampledPoints;

    xform = _cmsChain2Lab(ContextID, nProfiles, TYPE_CMYK_FLT, TYPE_Lab_DBL, Intents, hProfiles, BPC, AdaptationStates, dwFlags);
    if (xform == NULL) return NULL;

    SampledPoints = (cmsFloat32Number*) _cmsCalloc(ContextID, nPoints, sizeof(cmsFloat32Number));
    if (SampledPoints  == NULL) goto Error;

    for (i=0; i < nPoints; i++) {

        cmyk[0] = 0;
        cmyk[1] = 0;
        cmyk[2] = 0;
        cmyk[3] = (cmsFloat32Number) ((i * 100.0) / (nPoints-1));

        cmsDoTransform(xform, cmyk, &Lab, 1);
        SampledPoints[i]= (cmsFloat32Number) (1.0 - Lab.L / 100.0); // Negate K for easier operation
    }

    out = cmsBuildTabulatedToneCurveFloat(ContextID, nPoints, SampledPoints);

Error:

    cmsDeleteTransform(xform);
    if (SampledPoints) _cmsFree(ContextID, SampledPoints);

    return out;
}