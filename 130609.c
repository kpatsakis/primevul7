void sas_device_set_phy(struct domain_device *dev, struct sas_port *port)
{
	struct sas_ha_struct *ha;
	struct sas_phy *new_phy;

	if (!dev)
		return;

	ha = dev->port->ha;
	new_phy = sas_port_get_phy(port);

	/* pin and record last seen phy */
	spin_lock_irq(&ha->phy_port_lock);
	if (new_phy) {
		sas_port_put_phy(dev->phy);
		dev->phy = new_phy;
	}
	spin_unlock_irq(&ha->phy_port_lock);
}