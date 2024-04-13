static uint64_t megasas_get_sata_addr(uint16_t id)
{
    uint64_t addr = (0x1221ULL << 48);
    return addr | ((uint64_t)id << 24);
}