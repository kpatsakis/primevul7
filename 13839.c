SPICE_GNUC_VISIBLE void spice_server_vm_stop(SpiceServer *reds)
{
    reds->vm_running = FALSE;
    for (auto dev: reds->char_devices) {
        dev->stop();
    }
    reds_on_vm_stop(reds);
}