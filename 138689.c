static bool esp_is_version_5(void *opaque, int version_id)
{
    ESPState *s = ESP(opaque);

    version_id = MIN(version_id, s->mig_version_id);
    return version_id == 5;
}