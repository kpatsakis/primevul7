int reds_get_free_channel_id(RedsState *reds, uint32_t type)
{
    // this mark if some IDs are used.
    // The size of the array limits the possible id returned but
    // usually the IDs used for a channel type are not much.
    bool used_ids[256];

    unsigned n;

    // mark id used for the specific channel type
    memset(used_ids, 0, sizeof(used_ids));
    for (const auto channel: reds->channels) {
        if (channel->type() == type && channel->id() < SPICE_N_ELEMENTS(used_ids)) {
            used_ids[channel->id()] = true;
        }
    }

    // find first ID not marked as used
    for (n = 0; n < SPICE_N_ELEMENTS(used_ids); ++n) {
        if (!used_ids[n]) {
            return n;
        }
    }
    return -1;
}