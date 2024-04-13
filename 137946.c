static void nvme_instance_init(Object *obj)
{
    NvmeCtrl *n = NVME(obj);

    device_add_bootindex_property(obj, &n->namespace.blkconf.bootindex,
                                  "bootindex", "/namespace@1,0",
                                  DEVICE(obj));

    object_property_add(obj, "smart_critical_warning", "uint8",
                        nvme_get_smart_warning,
                        nvme_set_smart_warning, NULL, NULL);
}