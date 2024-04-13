static bool red_link_info_test_capability(const RedLinkInfo *link, uint32_t cap)
{
    const uint32_t *caps = red_link_info_get_caps(link);

    return test_capability(caps, link->link_mess->num_common_caps, cap);
}