static int spice_server_char_device_remove_interface(RedsState *reds, SpiceBaseInstance *sin)
{
    SpiceCharDeviceInstance* char_device =
            SPICE_UPCAST(SpiceCharDeviceInstance, sin);

    spice_debug("remove CHAR_DEVICE %s", char_device->subtype);
    if (strcmp(char_device->subtype, SUBTYPE_VDAGENT) == 0) {
        g_return_val_if_fail(char_device == reds->vdagent, -1);
        if (reds->vdagent) {
            reds_agent_remove(reds);
            reds->agent_dev->reset_dev_instance(NULL);
        }
    }

    if (char_device->st) {
        auto st = char_device->st;
        char_device->st = nullptr;
        reds_remove_char_device(reds, st);
    }
    return 0;
}