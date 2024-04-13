static void reds_start_auth_sasl(RedLinkInfo *link)
{
    if (!red_sasl_start_auth(link->stream, reds_handle_sasl_result, link)) {
        reds_link_free(link);
    }
}