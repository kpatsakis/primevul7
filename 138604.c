static void arcmsr_unmap_pciregion(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:{
		iounmap(acb->pmuA);
	}
	break;
	case ACB_ADAPTER_TYPE_B:{
		iounmap(acb->mem_base0);
		iounmap(acb->mem_base1);
	}

	break;
	case ACB_ADAPTER_TYPE_C:{
		iounmap(acb->pmuC);
	}
	break;
	case ACB_ADAPTER_TYPE_D:
		iounmap(acb->mem_base0);
		break;
	}
}