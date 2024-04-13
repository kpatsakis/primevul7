RedChannel *reds_find_channel(RedsState *reds, uint32_t type, uint32_t id)
{
    for (auto channel: reds->channels) {
        if (channel->type() == type && channel->id() == id) {
            return channel.get();
        }
    }
    return NULL;
}