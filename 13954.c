SPICE_GNUC_VISIBLE int spice_server_set_agent_copypaste(SpiceServer *reds, int enable)
{
    reds->config->agent_copypaste = enable;
    reds_update_agent_properties(reds);
    return 0;
}