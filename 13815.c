SPICE_GNUC_VISIBLE int spice_server_is_server_mouse(SpiceServer *reds)
{
    return reds->mouse_mode == SPICE_MOUSE_MODE_SERVER;
}