static void lcmsprofiledeleter(void *profile)
{
    cmsCloseProfile(profile);
}