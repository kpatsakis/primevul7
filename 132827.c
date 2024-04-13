static void maintenance_mode_update(struct ipmi_smi *intf)
{
	if (intf->handlers->set_maintenance_mode)
		intf->handlers->set_maintenance_mode(
			intf->send_info, intf->maintenance_mode_enable);
}