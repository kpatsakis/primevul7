void GfxState::setDisplayProfile(const GfxLCMSProfilePtr &localDisplayProfileA)
{
    localDisplayProfile = localDisplayProfileA;
    if (localDisplayProfile) {
        cmsHTRANSFORM transform;
        unsigned int nChannels;
        unsigned int localDisplayPixelType;

        localDisplayPixelType = getCMSColorSpaceType(cmsGetColorSpace(localDisplayProfile.get()));
        nChannels = getCMSNChannels(cmsGetColorSpace(localDisplayProfile.get()));
        // create transform from XYZ
        if ((transform = cmsCreateTransform(XYZProfile.get(), TYPE_XYZ_DBL, localDisplayProfile.get(), COLORSPACE_SH(localDisplayPixelType) | CHANNELS_SH(nChannels) | BYTES_SH(1), INTENT_RELATIVE_COLORIMETRIC, LCMS_FLAGS)) == nullptr) {
            error(errSyntaxWarning, -1, "Can't create Lab transform");
        } else {
            XYZ2DisplayTransformRelCol = std::make_shared<GfxColorTransform>(transform, INTENT_RELATIVE_COLORIMETRIC, PT_XYZ, localDisplayPixelType);
        }

        if ((transform = cmsCreateTransform(XYZProfile.get(), TYPE_XYZ_DBL, localDisplayProfile.get(), COLORSPACE_SH(localDisplayPixelType) | CHANNELS_SH(nChannels) | BYTES_SH(1), INTENT_ABSOLUTE_COLORIMETRIC, LCMS_FLAGS)) == nullptr) {
            error(errSyntaxWarning, -1, "Can't create Lab transform");
        } else {
            XYZ2DisplayTransformAbsCol = std::make_shared<GfxColorTransform>(transform, INTENT_ABSOLUTE_COLORIMETRIC, PT_XYZ, localDisplayPixelType);
        }

        if ((transform = cmsCreateTransform(XYZProfile.get(), TYPE_XYZ_DBL, localDisplayProfile.get(), COLORSPACE_SH(localDisplayPixelType) | CHANNELS_SH(nChannels) | BYTES_SH(1), INTENT_SATURATION, LCMS_FLAGS)) == nullptr) {
            error(errSyntaxWarning, -1, "Can't create Lab transform");
        } else {
            XYZ2DisplayTransformSat = std::make_shared<GfxColorTransform>(transform, INTENT_SATURATION, PT_XYZ, localDisplayPixelType);
        }

        if ((transform = cmsCreateTransform(XYZProfile.get(), TYPE_XYZ_DBL, localDisplayProfile.get(), COLORSPACE_SH(localDisplayPixelType) | CHANNELS_SH(nChannels) | BYTES_SH(1), INTENT_PERCEPTUAL, LCMS_FLAGS)) == nullptr) {
            error(errSyntaxWarning, -1, "Can't create Lab transform");
        } else {
            XYZ2DisplayTransformPerc = std::make_shared<GfxColorTransform>(transform, INTENT_PERCEPTUAL, PT_XYZ, localDisplayPixelType);
        }
    }
}