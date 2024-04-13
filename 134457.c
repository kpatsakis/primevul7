static void megasas_register_types(void)
{
    int i;

    type_register_static(&megasas_info);
    for (i = 0; i < ARRAY_SIZE(megasas_devices); i++) {
        const MegasasInfo *info = &megasas_devices[i];
        TypeInfo type_info = {};

        type_info.name = info->name;
        type_info.parent = TYPE_MEGASAS_BASE;
        type_info.class_data = (void *)info;
        type_info.class_init = megasas_class_init;

        type_register(&type_info);
    }
}