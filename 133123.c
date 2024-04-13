void license_free_product_info(PRODUCT_INFO* productInfo)
{
	if (productInfo->pbCompanyName != NULL)
		free(productInfo->pbCompanyName);

	if (productInfo->pbProductId != NULL)
		free(productInfo->pbProductId);

	free(productInfo);
}