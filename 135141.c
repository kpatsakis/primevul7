bool GfxICCBasedColorSpace::useGetCMYKLine() const
{
#ifdef USE_CMS
    return lineTransform != nullptr || alt->useGetCMYKLine();
#else
    return alt->useGetCMYKLine();
#endif
}