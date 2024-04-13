void reds_on_main_agent_tokens(RedsState *reds, MainChannelClient *mcc, uint32_t num_tokens)
{
    RedClient *client = mcc->get_client();
    if (!reds->vdagent) {
        return;
    }
    spice_assert(reds->vdagent->st);
    reds->vdagent->st->send_to_client_tokens_add((RedCharDeviceClientOpaque *)client,
                                                 num_tokens);
}