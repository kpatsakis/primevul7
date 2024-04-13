void RedCharDeviceVDIPort::remove_client(RedCharDeviceClientOpaque *opaque)
{
    RedClient *client = (RedClient *) opaque;
    client->get_main()->shutdown();
}