static void reds_link_free(RedLinkInfo *link)
{
    red_stream_free(link->stream);
    link->stream = NULL;

    g_free(link->link_mess);
    link->link_mess = NULL;

    BN_free(link->tiTicketing.bn);
    link->tiTicketing.bn = NULL;

    if (link->tiTicketing.rsa) {
        RSA_free(link->tiTicketing.rsa);
        link->tiTicketing.rsa = NULL;
    }

    g_free(link);
}