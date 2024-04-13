SPICE_GNUC_VISIBLE int spice_server_add_interface(SpiceServer *reds,
                                                  SpiceBaseInstance *sin)
{
    const SpiceBaseInterface *base_interface = sin->sif;

    if (strcmp(base_interface->type, SPICE_INTERFACE_KEYBOARD) == 0) {
        spice_debug("SPICE_INTERFACE_KEYBOARD");
        if (base_interface->major_version != SPICE_INTERFACE_KEYBOARD_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_KEYBOARD_MINOR) {
            spice_warning("unsupported keyboard interface");
            return -1;
        }
        if (reds->inputs_channel->set_keyboard(SPICE_UPCAST(SpiceKbdInstance, sin)) != 0) {
            return -1;
        }
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_MOUSE) == 0) {
        spice_debug("SPICE_INTERFACE_MOUSE");
        if (base_interface->major_version != SPICE_INTERFACE_MOUSE_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_MOUSE_MINOR) {
            spice_warning("unsupported mouse interface");
            return -1;
        }
        if (reds->inputs_channel->set_mouse(SPICE_UPCAST(SpiceMouseInstance, sin)) != 0) {
            return -1;
        }
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_QXL) == 0) {
        QXLInstance *qxl;

        spice_debug("SPICE_INTERFACE_QXL");
        if (base_interface->major_version != SPICE_INTERFACE_QXL_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_QXL_MINOR) {
            spice_warning("unsupported qxl interface");
            return -1;
        }

        qxl = SPICE_UPCAST(QXLInstance, sin);
        if (qxl->id < 0) {
            spice_warning("invalid QXL ID");
            return -1;
        }
        if (reds_find_channel(reds, SPICE_CHANNEL_DISPLAY, qxl->id)) {
            spice_warning("QXL ID already allocated");
            return -1;
        }
        red_qxl_init(reds, qxl);
        reds->qxl_instances.push_front(qxl);

        /* this function has to be called after the qxl is on the list
         * as QXLInstance clients expect the qxl to be on the list when
         * this callback is called. This as clients assume they can start the
         * qxl_instances. Also note that this should be the first callback to
         * be called. */
        red_qxl_attach_worker(qxl);
        red_qxl_set_compression_level(qxl, calc_compression_level(reds));
    } else if (strcmp(base_interface->type, SPICE_INTERFACE_TABLET) == 0) {
        SpiceTabletInstance *tablet = SPICE_UPCAST(SpiceTabletInstance, sin);
        spice_debug("SPICE_INTERFACE_TABLET");
        if (base_interface->major_version != SPICE_INTERFACE_TABLET_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_TABLET_MINOR) {
            spice_warning("unsupported tablet interface");
            return -1;
        }
        if (reds->inputs_channel->set_tablet(tablet) != 0) {
            return -1;
        }
        reds_update_mouse_mode(reds);
        if (reds->is_client_mouse_allowed) {
            reds->inputs_channel->set_tablet_logical_size(reds->monitor_mode.x_res, reds->monitor_mode.y_res);
        }

    } else if (strcmp(base_interface->type, SPICE_INTERFACE_PLAYBACK) == 0) {
        spice_debug("SPICE_INTERFACE_PLAYBACK");
        if (base_interface->major_version != SPICE_INTERFACE_PLAYBACK_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_PLAYBACK_MINOR) {
            spice_warning("unsupported playback interface");
            return -1;
        }
        snd_attach_playback(reds, SPICE_UPCAST(SpicePlaybackInstance, sin));

    } else if (strcmp(base_interface->type, SPICE_INTERFACE_RECORD) == 0) {
        spice_debug("SPICE_INTERFACE_RECORD");
        if (base_interface->major_version != SPICE_INTERFACE_RECORD_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_RECORD_MINOR) {
            spice_warning("unsupported record interface");
            return -1;
        }
        snd_attach_record(reds, SPICE_UPCAST(SpiceRecordInstance, sin));

    } else if (strcmp(base_interface->type, SPICE_INTERFACE_CHAR_DEVICE) == 0) {
        if (base_interface->major_version != SPICE_INTERFACE_CHAR_DEVICE_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_CHAR_DEVICE_MINOR) {
            spice_warning("unsupported char device interface");
            return -1;
        }
        spice_server_char_device_add_interface(reds, sin);

    } else if (strcmp(base_interface->type, SPICE_INTERFACE_MIGRATION) == 0) {
        spice_debug("SPICE_INTERFACE_MIGRATION");
        if (reds->migration_interface) {
            spice_warning("already have migration");
            return -1;
        }

        if (base_interface->major_version != SPICE_INTERFACE_MIGRATION_MAJOR ||
            base_interface->minor_version > SPICE_INTERFACE_MIGRATION_MINOR) {
            spice_warning("unsupported migration interface");
            return -1;
        }
        reds->migration_interface = SPICE_UPCAST(SpiceMigrateInstance, sin);
        reds->migration_interface->st = (SpiceMigrateState *)(intptr_t)1; // dummy pointer
    }

    return 0;
}