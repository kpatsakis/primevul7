int esp_pre_save(void *opaque)
{
    ESPState *s = ESP(object_resolve_path_component(
                      OBJECT(opaque), "esp"));

    s->mig_version_id = vmstate_esp.version_id;
    return 0;
}