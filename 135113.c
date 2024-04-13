GfxLCMSProfilePtr make_GfxLCMSProfilePtr(void *profile)
{
    if (profile == nullptr) {
        return GfxLCMSProfilePtr();
    }
    return GfxLCMSProfilePtr(profile, lcmsprofiledeleter);
}