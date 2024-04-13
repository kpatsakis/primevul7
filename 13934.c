SPICE_GNUC_VISIBLE int spice_server_set_agent_file_xfer(SpiceServer *reds, int enable)
{
    reds->config->agent_file_xfer = enable;
    reds_update_agent_properties(reds);
    return 0;
}