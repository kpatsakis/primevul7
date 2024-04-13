static void esp_fifo_push(Fifo8 *fifo, uint8_t val)
{
    if (fifo8_num_used(fifo) == fifo->capacity) {
        trace_esp_error_fifo_overrun();
        return;
    }

    fifo8_push(fifo, val);
}