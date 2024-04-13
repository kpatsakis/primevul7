RedCharDeviceVDIPort::read_one_msg_from_device()
{
    RedsState *reds;
    int n;

    reds = get_server();
    while (reds->vdagent) {
        switch (priv->read_state) {
        case VDI_PORT_READ_STATE_READ_HEADER:
            n = read(priv->receive_pos, priv->receive_len);
            if (!n) {
                return RedPipeItemPtr();
            }
            if ((priv->receive_len -= n)) {
                priv->receive_pos += n;
                return RedPipeItemPtr();
            }
            priv->message_receive_len = priv->vdi_chunk_header.size;
            priv->read_state = VDI_PORT_READ_STATE_GET_BUFF;
            /* fall through */
        case VDI_PORT_READ_STATE_GET_BUFF: {
            if (!(priv->current_read_buf = vdi_port_get_read_buf(this))) {
                return RedPipeItemPtr();
            }
            priv->receive_pos = priv->current_read_buf->data;
            priv->receive_len = MIN(priv->message_receive_len,
                                    sizeof(priv->current_read_buf->data));
            priv->current_read_buf->len = priv->receive_len;
            priv->message_receive_len -= priv->receive_len;
            priv->read_state = VDI_PORT_READ_STATE_READ_DATA;
        }
            /* fall through */
        case VDI_PORT_READ_STATE_READ_DATA: {
            n = read(priv->receive_pos, priv->receive_len);
            if (!n) {
                return RedPipeItemPtr();
            }
            if ((priv->receive_len -= n)) {
                priv->receive_pos += n;
                break;
            }
            auto dispatch_buf = std::move(priv->current_read_buf);
            priv->receive_pos = NULL;
            if (priv->message_receive_len == 0) {
                priv->read_state = VDI_PORT_READ_STATE_READ_HEADER;
                priv->receive_pos = (uint8_t *)&priv->vdi_chunk_header;
                priv->receive_len = sizeof(priv->vdi_chunk_header);
            } else {
                priv->read_state = VDI_PORT_READ_STATE_GET_BUFF;
            }
            switch (vdi_port_read_buf_process(this, *dispatch_buf)) {
            case AGENT_MSG_FILTER_OK:
                reds_adjust_agent_capabilities(reds, (VDAgentMessage *) dispatch_buf->data);
                return dispatch_buf;
            case AGENT_MSG_FILTER_PROTO_ERROR:
                reds_agent_remove(reds);
                /* fall through */
            case AGENT_MSG_FILTER_MONITORS_CONFIG:
                /* fall through */
            case AGENT_MSG_FILTER_DISCARD:
                dispatch_buf.reset();
            }
        }
        } /* END switch */
    } /* END while */
    return RedPipeItemPtr();
}