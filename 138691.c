static uint32_t esp_fifo_pop_buf(Fifo8 *fifo, uint8_t *dest, int maxlen)
{
    const uint8_t *buf;
    uint32_t n;

    if (maxlen == 0) {
        return 0;
    }

    buf = fifo8_pop_buf(fifo, maxlen, &n);
    if (dest) {
        memcpy(dest, buf, n);
    }

    return n;
}