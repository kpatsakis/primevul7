red_channel_capabilities_init_from_link_message(RedChannelCapabilities *caps,
                                                const SpiceLinkMess *link_mess)
{
    const uint8_t *raw_caps = (const uint8_t *)link_mess + link_mess->caps_offset;

    caps->num_common_caps = link_mess->num_common_caps;
    caps->common_caps = NULL;
    if (caps->num_common_caps) {
        caps->common_caps = (uint32_t*) g_memdup(raw_caps,
                                     link_mess->num_common_caps * sizeof(uint32_t));
    }
    caps->num_caps = link_mess->num_channel_caps;
    caps->caps = NULL;
    if (link_mess->num_channel_caps) {
        caps->caps = (uint32_t*) g_memdup(raw_caps + link_mess->num_common_caps * sizeof(uint32_t),
                              link_mess->num_channel_caps * sizeof(uint32_t));
    }
}