void reds_release_agent_data_buffer(RedsState *reds, uint8_t *buf)
{
    RedCharDeviceVDIPort *dev = reds->agent_dev.get();

    if (!dev->priv->recv_from_client_buf) {
        g_free(buf);
        return;
    }

    spice_assert(buf == dev->priv->recv_from_client_buf->buf + sizeof(VDIChunkHeader));
    /* if we pushed the buffer the buffer is attached to the channel so don't free it */
    if (!dev->priv->recv_from_client_buf_pushed) {
        RedCharDevice::write_buffer_release(dev,
                                            &dev->priv->recv_from_client_buf);
    }
    dev->priv->recv_from_client_buf = NULL;
    dev->priv->recv_from_client_buf_pushed = FALSE;
}