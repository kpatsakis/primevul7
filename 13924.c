void reds_marshall_device_display_info(RedsState *reds, SpiceMarshaller *m)
{
    uint32_t device_count = 0;
    void *device_count_ptr = spice_marshaller_add_uint32(m, device_count);

    // add the qxl devices to the message
    FOREACH_QXL_INSTANCE(reds, qxl) {
        device_count += red_qxl_marshall_device_display_info(qxl, m);
    }

    // add the stream devices to the message
    for (auto dev: reds->char_devices) {
        auto stream_dev = dynamic_cast<StreamDevice*>(dev.get());
        if (stream_dev) {
            const StreamDeviceDisplayInfo *info = stream_dev->get_device_display_info();
            size_t device_address_len = strlen(info->device_address) + 1;

            if (device_address_len == 1) {
                // the device info wasn't set (yet), don't send it
                continue;
            }

            int32_t channel_id = stream_dev->get_stream_channel_id();
            if (channel_id == -1) {
                g_warning("DeviceDisplayInfo set but no stream channel exists");
                continue;
            }

            spice_marshaller_add_uint32(m, channel_id);
            spice_marshaller_add_uint32(m, info->stream_id);
            spice_marshaller_add_uint32(m, info->device_display_id);
            spice_marshaller_add_uint32(m, device_address_len);
            spice_marshaller_add(m, (const uint8_t*) (void*) info->device_address, device_address_len);
            ++device_count;

            g_debug("   (stream) channel_id: %u monitor_id: %u, device_address: %s, "
                    "device_display_id: %u",
                    channel_id, info->stream_id, info->device_address,
                    info->device_display_id);
        }
    }
    spice_marshaller_set_uint32(m, device_count_ptr, device_count);
}