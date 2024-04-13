static void nvme_set_smart_warning(Object *obj, Visitor *v, const char *name,
                                   void *opaque, Error **errp)
{
    NvmeCtrl *n = NVME(obj);
    uint8_t value, old_value, cap = 0, index, event;

    if (!visit_type_uint8(v, name, &value, errp)) {
        return;
    }

    cap = NVME_SMART_SPARE | NVME_SMART_TEMPERATURE | NVME_SMART_RELIABILITY
          | NVME_SMART_MEDIA_READ_ONLY | NVME_SMART_FAILED_VOLATILE_MEDIA;
    if (NVME_CAP_PMRS(ldq_le_p(&n->bar.cap))) {
        cap |= NVME_SMART_PMR_UNRELIABLE;
    }

    if ((value & cap) != value) {
        error_setg(errp, "unsupported smart critical warning bits: 0x%x",
                   value & ~cap);
        return;
    }

    old_value = n->smart_critical_warning;
    n->smart_critical_warning = value;

    /* only inject new bits of smart critical warning */
    for (index = 0; index < NVME_SMART_WARN_MAX; index++) {
        event = 1 << index;
        if (value & ~old_value & event)
            nvme_smart_event(n, event);
    }
}