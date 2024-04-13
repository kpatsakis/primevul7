static RedCharDeviceWriteBuffer *vdagent_new_write_buffer(RedCharDeviceVDIPort *agent_dev,
                                                          uint32_t type,
                                                          size_t size,
                                                          bool use_token)
{
    uint32_t total_msg_size = sizeof(VDIChunkHeader) + sizeof(VDAgentMessage) + size;

    RedCharDeviceWriteBuffer *char_dev_buf;
        char_dev_buf = agent_dev->write_buffer_get_server(total_msg_size,
                                                          use_token);
    if (!char_dev_buf) {
        return NULL;  // no token was available
    }

    char_dev_buf->buf_used = total_msg_size;
    VDInternalBuf *internal_buf = (VDInternalBuf *)char_dev_buf->buf;
    internal_buf->chunk_header.port = VDP_SERVER_PORT;
    internal_buf->chunk_header.size = sizeof(VDAgentMessage) + size;
    internal_buf->header.protocol = VD_AGENT_PROTOCOL;
    internal_buf->header.type = type;
    internal_buf->header.opaque = 0;
    internal_buf->header.size = size;

    return char_dev_buf;
}