GfxColorTransform::GfxColorTransform(void *transformA, int cmsIntentA, unsigned int inputPixelTypeA, unsigned int transformPixelTypeA)
{
    transform = transformA;
    cmsIntent = cmsIntentA;
    inputPixelType = inputPixelTypeA;
    transformPixelType = transformPixelTypeA;
}