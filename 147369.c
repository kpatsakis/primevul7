qemuProcessReportLogError(qemuDomainLogContextPtr logCtxt,
                          const char *msgprefix)
{
    g_autofree char *logmsg = NULL;
    size_t max;

    max = VIR_ERROR_MAX_LENGTH - 1;
    max -= strlen(msgprefix);
    /* The length of the formatting string minus two '%s' */
    max -= strlen(_("%s: %s")) - 4;

    if (qemuProcessReadLog(logCtxt, &logmsg, max) < 0)
        return -1;

    virResetLastError();
    if (virStringIsEmpty(logmsg))
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s", msgprefix);
    else
        virReportError(VIR_ERR_INTERNAL_ERROR, _("%s: %s"), msgprefix, logmsg);

    return 0;
}