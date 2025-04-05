
#include "elf_getter.h" // all struct

#include <string.h> // metset()
#include <elf.h>	// ...

unsigned char
get_elf_symbol_char_type_x32(struct s_elf_symbol_header elf_symbol_header, struct s_elf_info elf_info)
{
/*
	R Read-only data symbol.
	r ?

	C Common symbol.

	A   Global absolute symbol.
	a   Local absolute symbol.

	Z   Weak bss Symbol 
	B   Global bss symbol.
	b   Local bss symbol.

	V   Weak Data Symbol
	D   Global data symbol.
	d   Local data symbol.

	f   Source file name symbol.

	L   Global thread-local symbol (TLS).
	l   Static thread-local symbol (TLS).

	W   Weak Text Symbol
	T   Global text symbol.
	t   Local text symbol.

	w   Weak Undefined Symbol
	U   Undefined symbol.
*/

	unsigned char type;
	unsigned char bind;

	type = ELF32_ST_TYPE(elf_symbol_header.symbol_type);
	bind = ELF32_ST_BIND(elf_symbol_header.symbol_type);



	if (SHN_UNDEF == elf_symbol_header.linked_section_header_index)
	{
		if (STB_WEAK == (bind & STB_WEAK)) {
			return 'w';
		}
		else {
			return 'U';
		}
	}

	Elf32_Shdr	*section_header;

	section_header = (Elf32_Shdr *)(elf_info.start_of_file + elf_info.section_header_offset);
	section_header += elf_symbol_header.linked_section_header_index;

	if (elf_symbol_header.linked_section_header_index != SHN_ABS &&
		elf_symbol_header.linked_section_header_index != SHN_COMMON &&
		elf_symbol_header.linked_section_header_index != SHN_XINDEX)
	{
		if (section_header->sh_type == SHT_NOBITS && section_header->sh_flags == (SHF_ALLOC | SHF_WRITE))
		{
			if (STB_WEAK == (bind & STB_WEAK)) {
				return 'Z';
			}
			else if (STB_GLOBAL == (bind & STB_GLOBAL)) {
				return 'B';
			}
			else if (STB_LOCAL == (bind & STB_LOCAL)) {
				return 'b';
			}
		}

		if (section_header->sh_flags == SHF_ALLOC)
		{
			if (STB_GLOBAL == (bind & STB_GLOBAL)) {
				return 'R';
			}
			else if (STB_LOCAL == (bind & STB_LOCAL)) {
				return 'r';
			}
		}

		if (section_header->sh_type == SHT_GROUP && STT_NOTYPE == type)
		{
			return 'n';
		}
	}

	if (STT_FUNC == type)
	{
		if (STB_WEAK == (bind & STB_WEAK)) {
			return 'V';
		}
		else if (STB_GLOBAL == (bind & STB_GLOBAL)) {
			return 'T';
		}
		else if (STB_LOCAL == (bind & STB_LOCAL)) {
			return 't';
		}
	}
	
	if (STT_SECTION == type)
	{
		// not sure how to endle this
		return 'S';

		
		// if (STB_WEAK == (bind & STB_WEAK)) {
		// 	return '3'; // idk
		// }
		// if (STB_LOCAL == (bind & STB_LOCAL)) {
		// 	return '1'; // idk
		// }
		// if (STB_GLOBAL == (bind & STB_GLOBAL)) {
		// 	return '2'; // idk
		// }
	}

	if (STT_FILE == type)
	{
		if (STB_GLOBAL == (bind & STB_GLOBAL)) {
			return 'A';
		}
		else if (STB_LOCAL == (bind & STB_LOCAL)) {
			return 'a';
		}
	}

	if (STT_COMMON == type)
	{
		return 'C';
	}
	
	if (STT_TLS == type)
	{
		if (STB_WEAK == (bind & STB_WEAK)) {
			return 'W';
		}
		else if (STB_GLOBAL == (bind & STB_GLOBAL)) {
			return 'L';
		}
		else if (STB_LOCAL == (bind & STB_LOCAL)) {
			return 'l';
		}
	}

	if (STT_OBJECT == type)
	{
		if (STB_WEAK == (bind & STB_WEAK)) {
			return 'V';
		}
		else if (STB_GLOBAL == (bind & STB_GLOBAL)) {
			return 'D';

		}
		else if (STB_LOCAL == (bind & STB_LOCAL)) {
			return 'd';
		}
	}

 	if (STT_NOTYPE == type)
 	{
 		if (STB_WEAK == (bind & STB_WEAK)) {
			return 'W';
		}
		else if (STB_GLOBAL == (bind & STB_GLOBAL)) {
			return 'D';
		}
		else if (STB_LOCAL == (bind & STB_LOCAL)) {
			return 'd';
		}
 	}

	// Not used
	// if (STT_LOPROC >= type && type <= STT_HIPROC)
	// if (STB_LOPROC >= bind && bind <= STB_HIPROC)
	return 'U';
}

int
get_elf_symbol_header_x32(struct s_elf_symbol_header *elf_symbol_header, size_t index,
	struct s_elf_section_header elf_section_header, struct s_elf_info elf_info)
{
	memset(elf_symbol_header, 0, sizeof(struct s_elf_symbol_header));
	if (index >= elf_section_header.size / elf_section_header.entry_size) {
		return -1;
	}
	Elf32_Sym *symbol_header;

	symbol_header = (Elf32_Sym *)(elf_info.start_of_file + elf_section_header.offset + (index * elf_section_header.entry_size));

	elf_symbol_header->name_index = symbol_header->st_name;
	elf_symbol_header->symbol_type = symbol_header->st_info;
	elf_symbol_header->symbol_visibility = symbol_header->st_other;
	elf_symbol_header->linked_section_header_index = symbol_header->st_shndx;
	elf_symbol_header->value = symbol_header->st_value;
	elf_symbol_header->size = symbol_header->st_size;

	if (elf_info.string_table_section_index == SHN_UNDEF) {
		elf_symbol_header->name = NULL;
	}
	else
	{
		Elf32_Shdr	*section_header;

		section_header = (Elf32_Shdr *)(elf_info.start_of_file + elf_info.section_header_offset);
		elf_symbol_header->name = section_header[elf_section_header.linked_section_header_index].sh_offset + elf_info.start_of_file;
		elf_symbol_header->name = elf_symbol_header->name + elf_symbol_header->name_index;
	}
	return 0;
}

int
get_elf_section_header_x32(struct s_elf_section_header *elf_section_header, size_t index, struct s_elf_info elf_info)
{
	memset(elf_section_header, 0, sizeof(struct s_elf_section_header));
	if (index >= elf_info.section_header_n) {
		return -1;
	}
	Elf32_Shdr	*section_header;

	section_header = (Elf32_Shdr *)(elf_info.start_of_file + elf_info.section_header_offset);
	if (elf_info.string_table_section_index == SHN_UNDEF) {
		elf_section_header->name = NULL;
	}
	else
	{
		elf_section_header->name = section_header[elf_info.string_table_section_index].sh_offset + elf_info.start_of_file;
		elf_section_header->name = elf_section_header->name + section_header[index].sh_name;
	}
	elf_section_header->type = section_header[index].sh_type;
	elf_section_header->flags = section_header[index].sh_flags;
	elf_section_header->addr = section_header[index].sh_addr;
	elf_section_header->offset = section_header[index].sh_offset;
	elf_section_header->size = section_header[index].sh_size;
	elf_section_header->linked_section_header_index = section_header[index].sh_link;
	elf_section_header->info = section_header[index].sh_info;
	elf_section_header->addr_align = section_header[index].sh_addralign;
	elf_section_header->entry_size = section_header[index].sh_entsize;
	return 0;
}

int
get_elf_program_header_x32(struct s_elf_program_header *elf_program_header, size_t index, struct s_elf_info elf_info)
{
	memset(elf_program_header, 0, sizeof(struct s_elf_program_header));
	if (index >= elf_info.program_header_n) {
		return -1;
	}
	Elf32_Phdr	*program_header;
	
	program_header = (Elf32_Phdr *)(elf_info.start_of_file + elf_info.program_header_offset);
	elf_program_header->type = program_header[index].p_type;
	elf_program_header->flag = program_header[index].p_flags;
	elf_program_header->offset = program_header[index].p_offset;
	elf_program_header->virtual_addr = program_header[index].p_vaddr;
	elf_program_header->physical_addr = program_header[index].p_paddr;
	elf_program_header->file_size = program_header[index].p_filesz;
	elf_program_header->memory_size = program_header[index].p_memsz;
	elf_program_header->align = program_header[index].p_align;
	return 0;
}

void
get_elf_info_x32(struct s_elf_info *elf_info)
{
	Elf32_Ehdr	*header;

	header = (Elf32_Ehdr *)elf_info->start_of_file;
	elf_info->elf_class = ELFCLASS32;
	elf_info->data_encoding = header->e_ident[EI_DATA];
	elf_info->version = header->e_ident[EI_VERSION];
	elf_info->os_abi = header->e_ident[EI_OSABI];
	elf_info->abi_version = header->e_ident[EI_ABIVERSION];

	elf_info->file_type = header->e_type;
	elf_info->machine_architecture = header->e_machine;
	elf_info->flag = header->e_flags;

	elf_info->entry_point = header->e_entry;

	Elf32_Shdr	*section_header;

	section_header = (Elf32_Shdr *)(elf_info->start_of_file + header->e_shoff);
	if (header->e_shstrndx >= SHN_LORESERVE) {
		elf_info->string_table_section_index = section_header->sh_link;
	}
	else {
		elf_info->string_table_section_index = header->e_shstrndx;
	}

	elf_info->program_header_offset = header->e_phoff;
	elf_info->program_header_size = header->e_phentsize;
	if (header->e_phnum >= PN_XNUM) {
		elf_info->program_header_n = section_header->sh_info;
	}
	else {
		elf_info->program_header_n = header->e_phnum;
	}	

	elf_info->section_header_offset = header->e_shoff;
	elf_info->section_header_size = header->e_shentsize;
	if (header->e_shnum >= SHN_LORESERVE) {
		elf_info->section_header_n = section_header->sh_size;
	}
	else {
		elf_info->section_header_n = header->e_shnum;
	}
}