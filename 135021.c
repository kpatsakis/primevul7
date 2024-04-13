void GfxColorTransform::doTransform(void *in, void *out, unsigned int size)
{
    cmsDoTransform(transform, in, out, size);
}