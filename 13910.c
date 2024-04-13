attach_to_red_agent(RedsState *reds, SpiceCharDeviceInstance *sin)
{
    RedCharDeviceVDIPort *dev = reds->agent_dev.get();
    SpiceCharDeviceInterface *sif;

    dev->priv->agent_attached = true;
    dev->reset_dev_instance(sin);

    reds->vdagent = sin;
    reds_update_mouse_mode(reds);

    sif = spice_char_device_get_interface(sin);
    if (sif->state) {
        sif->state(sin, 1);
    }

    if (!reds_main_channel_connected(reds)) {
        return reds->agent_dev;
    }

    dev->priv->read_filter.discard_all = FALSE;
    dev->priv->plug_generation++;

    if (dev->priv->mig_data ||
        reds->main_channel->is_waiting_for_migrate_data()) {
        /* Migration in progress (code is running on the destination host):
         * 1.  Add the client to spice char device, if it was not already added.
         * 2.a If this (qemu-kvm state load side of migration) happens first
         *     then wait for spice migration data to arrive. Otherwise
         * 2.b If this happens second ==> we already have spice migrate data
         *     then restore state
         */
        RedCharDeviceClientOpaque *client_opaque =
            (RedCharDeviceClientOpaque *) reds_get_client(reds);
        if (!dev->client_exists(client_opaque)) {
            int client_added;

            client_added = dev->client_add(client_opaque, TRUE,
                                           REDS_VDI_PORT_NUM_RECEIVE_BUFFS,
                                           REDS_AGENT_WINDOW_SIZE, ~0, TRUE);

            if (!client_added) {
                spice_warning("failed to add client to agent");
                reds_disconnect(reds);
            }
        }

        if (dev->priv->mig_data) {
            spice_debug("restoring dev from stored migration data");
            spice_assert(dev->priv->plug_generation == 1);
            reds_agent_state_restore(reds, dev->priv->mig_data);
            g_free(dev->priv->mig_data);
            dev->priv->mig_data = NULL;
        }
        else {
            spice_debug("waiting for migration data");
        }
    } else {
        /* we will associate the client with the char device, upon reds_on_main_agent_start,
         * in response to MSGC_AGENT_START */
        reds->main_channel->push_agent_connected();
    }

    return reds->agent_dev;
}