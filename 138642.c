static uint8_t esp_fifo_pop(Fifo8 *fifo)
{
    if (fifo8_is_empty(fifo)) {
        return 0;
    }

    return fifo8_pop(fifo);
}