qemuProcessMonitorLogFree(void *opaque)
{
    qemuDomainLogContextPtr logCtxt = opaque;
    g_clear_object(&logCtxt);
}