bool GfxICCBasedColorSpace::useGetDeviceNLine() const
{
#ifdef USE_CMS
    return lineTransform != nullptr || alt->useGetDeviceNLine();
#else
    return alt->useGetDeviceNLine();
#endif
}