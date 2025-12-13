#include "paging.h"

void default_page_directory(M_PAGE_ENTRY* directory)
{
	//null first page.. catches kernel-level null pointer dereferences
	for (int i = 0; i < 1024; i++) {map_page(directory, i, 0x0, 0x0 

	
}
