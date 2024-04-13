SPICE_GNUC_VISIBLE int spice_server_remove_interface(SpiceBaseInstance *sin)
{
    RedsState *reds;
    const SpiceBaseInterface *base_interface;

    g_return_val_if_fail(sin != NULL, -1);

    base_interface = sin->sif;
    if (strcmp(base_interface->type, SPICE_INTERFACE_TABLET) == 0) {
        SpiceTabletInstance *tablet = SPICE_UPCAST(SpiceTabletInstance, sin);
        g_return_val_if_fail(tablet->st != NULL, -1);
        reds = spice_tablet_state_get_server(tablet->st);
        spice_debug("remove SPICE_INTERFACE_TABLET");
        reds->inputs_channel->detach_tablet(tablet);
        reds_update_mouse_mode(reds);
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_PLAYBACK) == 0) {
        spice_debug("remove SPICE_INTERFACE_PLAYBACK");
        snd_detach_playback(SPICE_UPCAST(SpicePlaybackInstance, sin));
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_RECORD) == 0) {
        spice_debug("remove SPICE_INTERFACE_RECORD");
        snd_detach_record(SPICE_UPCAST(SpiceRecordInstance, sin));
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_CHAR_DEVICE) == 0) {
        SpiceCharDeviceInstance *char_device = SPICE_UPCAST(SpiceCharDeviceInstance, sin);
        g_return_val_if_fail(char_device->st != NULL, -1);
        reds = char_device->st->get_server();
        return spice_server_char_device_remove_interface(reds, sin);
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_QXL) == 0) {
        QXLInstance *qxl;

        qxl = SPICE_UPCAST(QXLInstance, sin);
        g_return_val_if_fail(qxl->st != NULL, -1);
        reds = red_qxl_get_server(qxl->st);
        reds->qxl_instances.remove(qxl); // XXX owning
        red_qxl_destroy(qxl);
    } else {
        spice_warning("VD_INTERFACE_REMOVING unsupported");
        return -1;
    }

    return 0;
}