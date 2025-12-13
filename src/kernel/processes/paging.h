typedef unsigned int M_PAGE_ENTRY;
typedef unsigned int M_PHYSICAL_PAGE;
typedef unsigned int M_VIRTUAL_ADDR;
typedef unsigned int M_ENTRY_FLAGS;

void default_page_directory(M_PAGE_ENTRY* directory);
void create_process_directory(M_PAGE_ENTRY* directory);

M_PHYSICAL_PAGE alloc_page();
void map_page(M_PAGE_ENTRY* directory, unsigned int entry, M_VIRTUAL_ADDR vaddr, M_PHYSICAL_PAGE page, M_ENTRY_FLAGS flags);
