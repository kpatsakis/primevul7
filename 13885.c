void RedCharDeviceVDIPort::send_msg_to_client(RedPipeItem *msg, RedCharDeviceClientOpaque *opaque)
{
    RedClient *client = (RedClient *) opaque;
    RedVDIReadBuf *agent_data_buf = static_cast<RedVDIReadBuf*>(msg);

    client->get_main()->push_agent_data(red::shared_ptr<RedAgentDataPipeItem>(agent_data_buf));
}