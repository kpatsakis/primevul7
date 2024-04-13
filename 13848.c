SPICE_GNUC_VISIBLE void spice_server_vm_start(SpiceServer *reds)
{
    reds->vm_running = TRUE;
    for (auto dev: reds->char_devices) {
        dev->start();
    }
    reds_on_vm_start(reds);
}