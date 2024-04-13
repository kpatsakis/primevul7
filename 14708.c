static inline struct pxa3xx_gcu_priv *to_pxa3xx_gcu_priv(struct file *file)
{
	struct miscdevice *dev = file->private_data;
	return container_of(dev, struct pxa3xx_gcu_priv, misc_dev);
}