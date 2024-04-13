RedCharDeviceVDIPort::~RedCharDeviceVDIPort()
{
    /* make sure we have no other references to RedVDIReadBuf buffers */
    reset();
    priv->current_read_buf.reset(); // needed to pass the assert below
    g_free(priv->mig_data);
    spice_extra_assert(priv->num_read_buf == 0);
}