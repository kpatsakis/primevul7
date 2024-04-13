sync_entryuuid2uuid(const char *entryuuid)
{
    char *uuid;
    char u[17] = {0};

    u[0] = slapi_str_to_u8(entryuuid);
    u[1] = slapi_str_to_u8(entryuuid + 2);
    u[2] = slapi_str_to_u8(entryuuid + 4);
    u[3] = slapi_str_to_u8(entryuuid + 6);

    u[4] = slapi_str_to_u8(entryuuid + 9);
    u[5] = slapi_str_to_u8(entryuuid + 11);

    u[6] = slapi_str_to_u8(entryuuid + 14);
    u[7] = slapi_str_to_u8(entryuuid + 16);

    u[8] = slapi_str_to_u8(entryuuid + 19);
    u[9] = slapi_str_to_u8(entryuuid + 21);

    u[10] = slapi_str_to_u8(entryuuid + 24);
    u[11] = slapi_str_to_u8(entryuuid + 26);
    u[12] = slapi_str_to_u8(entryuuid + 28);
    u[13] = slapi_str_to_u8(entryuuid + 30);
    u[14] = slapi_str_to_u8(entryuuid + 32);
    u[15] = slapi_str_to_u8(entryuuid + 34);

    uuid = slapi_ch_malloc(sizeof(u));
    memcpy(uuid, u, sizeof(u));

    return (uuid);
}