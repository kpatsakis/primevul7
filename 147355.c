qemuProcessIncomingDefFree(qemuProcessIncomingDefPtr inc)
{
    if (!inc)
        return;

    g_free(inc->address);
    g_free(inc->launchURI);
    g_free(inc->deferredURI);
    g_free(inc);
}