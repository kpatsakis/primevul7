static void esp_init(Object *obj)
{
    ESPState *s = ESP(obj);

    fifo8_create(&s->fifo, ESP_FIFO_SZ);
    fifo8_create(&s->cmdfifo, ESP_CMDFIFO_SZ);
}