qemuProcessMonitorReportLogError(qemuMonitorPtr mon G_GNUC_UNUSED,
                                 const char *msg,
                                 void *opaque)
{
    qemuDomainLogContextPtr logCtxt = opaque;
    qemuProcessReportLogError(logCtxt, msg);
}