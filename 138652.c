static bool esp_mem_accepts(void *opaque, hwaddr addr,
                            unsigned size, bool is_write,
                            MemTxAttrs attrs)
{
    return (size == 1) || (is_write && size == 4);
}