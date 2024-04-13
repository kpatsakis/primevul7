SPICE_GNUC_VISIBLE int spice_server_set_agent_mouse(SpiceServer *reds, int enable)
{
    reds->config->agent_mouse = enable;
    reds_update_mouse_mode(reds);
    return 0;
}