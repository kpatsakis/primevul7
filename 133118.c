PRODUCT_INFO* license_new_product_info()
{
	PRODUCT_INFO* productInfo;

	productInfo = (PRODUCT_INFO*) malloc(sizeof(PRODUCT_INFO));

	productInfo->dwVersion = 0;
	productInfo->cbCompanyName = 0;
	productInfo->pbCompanyName = NULL;
	productInfo->cbProductId = 0;
	productInfo->pbProductId = NULL;

	return productInfo;
}