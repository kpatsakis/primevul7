static void reds_get_spice_ticket(RedLinkInfo *link)
{
    red_stream_async_read(link->stream,
                          (uint8_t *)&link->tiTicketing.encrypted_ticket.encrypted_data,
                          link->tiTicketing.rsa_size, reds_handle_ticket, link);
}