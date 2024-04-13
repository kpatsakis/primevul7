static void esp_lower_drq(ESPState *s)
{
    qemu_irq_lower(s->irq_data);
    trace_esp_lower_drq();
}