GfxICCBasedColorSpace::~GfxICCBasedColorSpace()
{
    delete alt;
#ifdef USE_CMS
    if (psCSA)
        gfree(psCSA);
#endif
}