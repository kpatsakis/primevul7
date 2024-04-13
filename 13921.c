SPICE_GNUC_VISIBLE void spice_server_set_uuid(SpiceServer *s, const uint8_t uuid[16])
{
    memcpy(s->config->spice_uuid, uuid, sizeof(s->config->spice_uuid));
    s->config->spice_uuid_is_set = TRUE;
}