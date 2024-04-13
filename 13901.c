void reds_register_channel(RedsState *reds, RedChannel *channel)
{
    spice_assert(reds);

    uint32_t this_type = channel->type();
    uint32_t this_id = channel->id();
    if (spice_extra_checks) {
        g_assert(reds_find_channel(reds, this_type, this_id) == NULL);
    } else {
        g_warn_if_fail(reds_find_channel(reds, this_type, this_id) == NULL);
    }
    reds->channels.push_front(red::shared_ptr<RedChannel>(channel));
    // create new channel in the client if possible
    reds->main_channel->registered_new_channel(channel);
}