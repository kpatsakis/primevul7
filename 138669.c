static void esp_soft_reset(ESPState *s)
{
    qemu_irq_lower(s->irq);
    qemu_irq_lower(s->irq_data);
    esp_hard_reset(s);
}