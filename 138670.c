static void esp_raise_drq(ESPState *s)
{
    qemu_irq_raise(s->irq_data);
    trace_esp_raise_drq();
}