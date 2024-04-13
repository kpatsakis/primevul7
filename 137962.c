static void nvme_get_smart_warning(Object *obj, Visitor *v, const char *name,
                                   void *opaque, Error **errp)
{
    NvmeCtrl *n = NVME(obj);
    uint8_t value = n->smart_critical_warning;

    visit_type_uint8(v, name, &value, errp);
}