SPICE_GNUC_VISIBLE int spice_server_get_num_clients(SpiceServer *reds)
{
    return reds ? reds->clients.size() : 0;
}