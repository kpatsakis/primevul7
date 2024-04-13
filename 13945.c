static const uint32_t *red_link_info_get_caps(const RedLinkInfo *link)
{
    const uint8_t *caps_start = (const uint8_t *)link->link_mess;

    return (const uint32_t *)(caps_start + link->link_mess->caps_offset);
}