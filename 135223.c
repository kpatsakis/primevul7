GfxColorTransform::~GfxColorTransform()
{
    cmsDeleteTransform(transform);
}