void reds_unregister_channel(RedsState *reds, RedChannel *channel)
{
    reds->channels.remove(red::shared_ptr<RedChannel>(channel));
}