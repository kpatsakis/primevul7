static red::shared_ptr<RedVDIReadBuf> vdi_port_get_read_buf(RedCharDeviceVDIPort *dev)
{
    if (dev->priv->num_read_buf >= REDS_VDI_PORT_NUM_RECEIVE_BUFFS) {
        return red::shared_ptr<RedVDIReadBuf>();
    }

    dev->priv->num_read_buf++;
    return vdi_read_buf_new(dev);
}