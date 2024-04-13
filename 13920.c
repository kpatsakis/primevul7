static bool channel_supports_multiple_clients(const RedChannel *channel)
{
    switch (channel->type()) {
    case SPICE_CHANNEL_MAIN:
    case SPICE_CHANNEL_DISPLAY:
    case SPICE_CHANNEL_CURSOR:
    case SPICE_CHANNEL_INPUTS:
        return TRUE;
    }
    return FALSE;
}