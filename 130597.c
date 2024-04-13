static void sas_destruct_ports(struct asd_sas_port *port)
{
	struct sas_port *sas_port, *p;

	list_for_each_entry_safe(sas_port, p, &port->sas_port_del_list, del_list) {
		list_del_init(&sas_port->del_list);
		sas_port_delete(sas_port);
	}
}