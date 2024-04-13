static red::shared_ptr<RedVDIReadBuf> vdi_read_buf_new(RedCharDeviceVDIPort *dev)
{
    auto buf = red::make_shared<RedVDIReadBuf>();
    buf->dev = dev;
    return buf;
}