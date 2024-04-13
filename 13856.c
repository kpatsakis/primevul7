bool reds_handle_migrate_data(RedsState *reds, MainChannelClient *mcc,
                              SpiceMigrateDataMain *mig_data, uint32_t size)
{
    RedCharDeviceVDIPort *agent_dev = reds->agent_dev.get();

    spice_debug("main-channel: got migrate data");
    /*
     * Now that the client has switched to the target server, if main_channel
     * controls the mm-time, we update the client's mm-time.
     * (MSG_MAIN_INIT is not sent for a migrating connection)
     */
    if (reds->mm_time_enabled) {
        reds_send_mm_time(reds);
    }
    if (mig_data->agent_base.connected) {
        if (agent_dev->priv->agent_attached) { // agent was attached before migration data has arrived
            if (!reds->vdagent) {
                spice_assert(agent_dev->priv->plug_generation > 0);
                reds->main_channel->push_agent_disconnected();
                spice_debug("agent is no longer connected");
            } else {
                if (agent_dev->priv->plug_generation > 1) {
                    /* red_char_device_state_reset takes care of not making the device wait for migration data */
                    spice_debug("agent has been detached and reattached before receiving migration data");
                    reds->main_channel->push_agent_disconnected();
                    reds->main_channel->push_agent_connected();
                } else {
                    spice_debug("restoring state from mig_data");
                    return reds_agent_state_restore(reds, mig_data);
                }
            }
        } else {
            /* restore agent state when the agent gets attached */
            spice_debug("saving mig_data");
            spice_assert(agent_dev->priv->plug_generation == 0);
            agent_dev->priv->mig_data = (SpiceMigrateDataMain*) g_memdup(mig_data, size);
        }
    } else {
        spice_debug("agent was not attached on the source host");
        if (reds->vdagent) {
            RedCharDeviceClientOpaque *client_opaque =
                (RedCharDeviceClientOpaque *) mcc->get_client();
            /* red_char_device_client_remove disables waiting for migration data */
            agent_dev->client_remove(client_opaque);
            reds->main_channel->push_agent_connected();
        }
    }

    return TRUE;
}