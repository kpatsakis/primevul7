int GfxState::getCmsRenderingIntent()
{
    const char *intent = getRenderingIntent();
    int cmsIntent = INTENT_RELATIVE_COLORIMETRIC;
    if (intent) {
        if (strcmp(intent, "AbsoluteColorimetric") == 0) {
            cmsIntent = INTENT_ABSOLUTE_COLORIMETRIC;
        } else if (strcmp(intent, "Saturation") == 0) {
            cmsIntent = INTENT_SATURATION;
        } else if (strcmp(intent, "Perceptual") == 0) {
            cmsIntent = INTENT_PERCEPTUAL;
        }
    }
    return cmsIntent;
}