void RedCharDeviceVDIPort::send_tokens_to_client(RedCharDeviceClientOpaque *opaque, uint32_t tokens)
{
    RedClient *client = (RedClient *) opaque;
    client->get_main()->push_agent_tokens(tokens);
}