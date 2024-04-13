RedVDIReadBuf::~RedVDIReadBuf()
{
    dev->priv->num_read_buf--;

    /* read_one_msg_from_vdi_port may have never completed because we
       reached buffer limit. So we call it again so it can complete its work if
       necessary. Note that since we can be called from red_char_device_wakeup
       this can cause recursion, but we have protection for that */
    if (dev->priv->agent_attached) {
       dev->wakeup();
    }
}