bool GfxICCBasedColorSpace::useGetRGBLine() const
{
#ifdef USE_CMS
    return lineTransform != nullptr || alt->useGetRGBLine();
#else
    return alt->useGetRGBLine();
#endif
}