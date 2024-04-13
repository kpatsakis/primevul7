static void sysbus_esp_init(Object *obj)
{
    SysBusESPState *sysbus = SYSBUS_ESP(obj);

    object_initialize_child(obj, "esp", &sysbus->esp, TYPE_ESP);
}