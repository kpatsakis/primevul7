static bool reds_find_client(RedsState *reds, RedClient *client)
{
    for (auto list_client: reds->clients) {
        if (list_client == client) {
            return TRUE;
        }
    }
    return FALSE;
}