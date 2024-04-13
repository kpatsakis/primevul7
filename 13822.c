SPICE_GNUC_VISIBLE void spice_server_char_device_wakeup(SpiceCharDeviceInstance* sin)
{
    if (!sin->st) {
        spice_warning("no RedCharDevice attached to instance %p", sin);
        return;
    }
    sin->st->wakeup();
}