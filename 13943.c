static RedClient *reds_get_client(RedsState *reds)
{
    spice_assert(reds->clients.size() <= 1);

    if (reds->clients.empty()) {
        return NULL;
    }

    return *reds->clients.begin();
}