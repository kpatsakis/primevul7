static void esp_finalize(Object *obj)
{
    ESPState *s = ESP(obj);

    fifo8_destroy(&s->fifo);
    fifo8_destroy(&s->cmdfifo);
}