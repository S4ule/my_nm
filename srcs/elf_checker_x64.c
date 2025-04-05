

#include "nm.h"		// struct s_file_info,
#include <stdio.h>	// printf
#include <elf.h>	// ...

static int
elf_program_header_check_x64(Elf64_Ehdr *header, struct s_file_info file_info)
{
	Elf64_Phdr	*program_header;
	Elf64_Shdr	*section_header;
	size_t		e_phnum;

	// Program Header
	if (header->e_phentsize != sizeof(Elf64_Phdr) && header->e_phnum != 0) {
		printf("nm: '%s': invalid program header size in ELF header\n", file_info.path);
		return 1;
	}
	if (header->e_phoff > file_info.size) {
		printf("nm: '%s': invalid program header offset in ELF header\n", file_info.path);
		return 1;
	}
	if ((header->e_phnum == 0 && header->e_phoff != 0) ||
		(header->e_phnum != 0 && header->e_phoff == 0) )
	{
		printf("nm: '%s': invalid program header info in ELF header\n", file_info.path);
		return 1;
	}

	if (header->e_phnum < PN_XNUM &&
		header->e_phnum * header->e_phentsize + header->e_phoff > file_info.size )
	{
		printf("nm: '%s': invalid program header table in ELF header\n", file_info.path);
		return 1;
	}
	program_header = (Elf64_Phdr *)(file_info.data + header->e_phoff);

	section_header = (Elf64_Shdr *)(file_info.data + header->e_shoff);
	if (header->e_phnum >= PN_XNUM) {
		e_phnum = section_header->sh_info;
	}
	else {
		e_phnum = header->e_phnum;
	}


	// Program Header Table
	for (size_t i = 0; i < e_phnum; i++)
	{
		if (program_header[i].p_type != PT_NULL &&
			program_header[i].p_type != PT_LOAD &&
			program_header[i].p_type != PT_DYNAMIC &&
			program_header[i].p_type != PT_INTERP &&
			program_header[i].p_type != PT_NOTE &&
			program_header[i].p_type != PT_SHLIB &&
			program_header[i].p_type != PT_PHDR &&
			program_header[i].p_type != PT_TLS &&
			program_header[i].p_type != PT_GNU_STACK &&
			program_header[i].p_type != PT_GNU_PROPERTY &&
			program_header[i].p_type != PT_GNU_EH_FRAME &&
			program_header[i].p_type != PT_GNU_RELRO &&
			program_header[i].p_type < PT_LOPROC )
		{
			printf("nm: '%s': invalid program type in program table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (program_header[i].p_offset > file_info.size ||
			program_header[i].p_offset + program_header[i].p_filesz > file_info.size) {
			printf("nm: '%s': invalid offset in program table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (program_header[i].p_type == PT_LOAD &&
			program_header[i].p_memsz < program_header[i].p_filesz )
		{
			printf("nm: '%s': invalid .p_memsz in PT_LOAD type program in program table entry %lu\n", file_info.path, i);
			return 1;
		}

		if ((program_header[i].p_flags & (!(PF_X | PF_W | PF_R))) != 0) {
			printf("nm: '%s': invalid .p_flags in program table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (program_header[i].p_type == PT_LOAD &&
			program_header[i].p_align != 0 &&
			program_header[i].p_vaddr % file_info.page_size != program_header[i].p_offset % file_info.page_size)
		{
			printf("nm: '%s': invalid .p_align in PT_LOAD type program in program table entry %lu\n", file_info.path, i);
			return 1;
		}
	}
	return 0;
}

static int
elf_section_header_check_x64(Elf64_Ehdr *header, struct s_file_info file_info)
{
	Elf64_Shdr	*section_header;
	size_t		e_shnum;

	// Section Header
	if (header->e_shentsize != sizeof(Elf64_Shdr) && header->e_shnum != 0) {
		printf("nm: '%s': invalid section header size in ELF header\n", file_info.path);
		return 1;
	}
	if (header->e_shoff > file_info.size) {
		printf("nm: '%s': invalid section header offset in ELF header\n", file_info.path);
		return 1;
	}
	if ((header->e_shnum == 0 && header->e_shoff != 0) ||
		(header->e_shnum != 0 && header->e_shoff == 0) )
	{
		printf("nm: '%s': invalid section header info in ELF header\n", file_info.path);
		return 1;
	}
	if (header->e_shnum < SHN_LORESERVE &&
		header->e_shnum * header->e_shentsize + header->e_shoff > file_info.size )
	{
		printf("nm: '%s': invalid section header table in ELF header\n", file_info.path);
		return 1;
	}
	section_header = (Elf64_Shdr *)(file_info.data + header->e_shoff);

	if (header->e_shnum >= SHN_LORESERVE) {
		e_shnum = section_header->sh_size;
	}
	else {
		e_shnum = header->e_shnum;
	}

	// Section Header Table
	for (size_t i = 0; i < e_shnum; i++)
	{
		if (section_header[i].sh_type != SHT_NULL &&
			section_header[i].sh_type != SHT_PROGBITS &&
			section_header[i].sh_type != SHT_SYMTAB &&
			section_header[i].sh_type != SHT_DYNSYM &&
			section_header[i].sh_type != SHT_STRTAB &&
			section_header[i].sh_type != SHT_RELA &&
			section_header[i].sh_type != SHT_HASH &&
			section_header[i].sh_type != SHT_DYNAMIC &&
			section_header[i].sh_type != SHT_NOTE &&
			section_header[i].sh_type != SHT_NOBITS &&
			section_header[i].sh_type != SHT_REL &&
			section_header[i].sh_type != SHT_SHLIB &&
			section_header[i].sh_type != SHT_GROUP &&
			section_header[i].sh_type != SHT_INIT_ARRAY &&
			section_header[i].sh_type != SHT_FINI_ARRAY &&
			section_header[i].sh_type != SHT_PREINIT_ARRAY &&
			section_header[i].sh_type != SHT_SUNW_COMDAT &&
			section_header[i].sh_type != SHT_SUNW_move &&
			section_header[i].sh_type != SHT_SUNW_syminfo &&
			section_header[i].sh_type != SHT_GNU_HASH &&
			section_header[i].sh_type != SHT_GNU_verdef &&
			section_header[i].sh_type != SHT_GNU_verneed &&
			section_header[i].sh_type != SHT_GNU_versym &&
			section_header[i].sh_type != SHT_LOUSER &&
			section_header[i].sh_type != SHT_HIUSER &&
			section_header[i].sh_type < SHT_LOPROC )
		{
			printf("nm: '%s': invalid .sh_type in section table entry %lu\n", file_info.path, i);
			return 1;
		}
		if (section_header[i].sh_type == SHT_NULL) {
			continue ;
		}

		if ((section_header[i].sh_flags & (!(SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR | SHF_MASKPROC))) != 0 ) {
			printf("nm: '%s': invalid .sh_flags in section table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (section_header[i].sh_type != SHT_NOBITS &&
			(section_header[i].sh_offset > file_info.size ||
			 section_header[i].sh_offset + section_header[i].sh_size > file_info.size ) )
		{
			printf("nm: '%s': invalid offset in section table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (section_header[i].sh_link > header->e_shnum) {
			printf("nm: '%s': invalid .sh_link in section table entry %lu\n", file_info.path, i);
			return 1;
		}

		if (section_header[i].sh_addralign > 1 &&
			section_header[i].sh_addr % section_header[i].sh_addralign != 0) {
			printf("nm: '%s': invalid .sh_addralign in section table entry %lu\n", file_info.path, i);
			return 1;
		}
	}
	return 0;
}

static int
elf_string_header_check_x64(Elf64_Ehdr *header, struct s_file_info file_info)
{
	Elf64_Shdr		*section_header;
	Elf64_Shdr		*string_section_header;
	size_t			e_shnum;
	size_t			name_len;
	unsigned char	*name_ptr;

	section_header = (Elf64_Shdr *)(file_info.data + header->e_shoff);
	if (header->e_shstrndx >= SHN_LORESERVE) {
		string_section_header = section_header + section_header->sh_link;
	}
	else {
		string_section_header = section_header + header->e_shstrndx;
	}

	if (header->e_shnum >= SHN_LORESERVE) {
		e_shnum = section_header->sh_size;
	}
	else {
		e_shnum = header->e_shnum;
	}

	name_ptr = (unsigned char *)(string_section_header->sh_offset + file_info.data);
	for (size_t i = 0; i < e_shnum; i++)
	{
		name_len = section_header[i].sh_name;
		while (name_ptr[name_len] != '\0')
		{
			if (name_len >= string_section_header->sh_size) {
				printf("nm: '%s': invalid .sh_name in section table entry %lu\n", file_info.path, i);
				return 1;
			}
			name_len++;
		}
	}
	return 0;
}

static int
elf_symbol_table_check_x64(Elf64_Ehdr *header, struct s_file_info file_info)
{
	Elf64_Shdr		*section_header;
	Elf64_Shdr		*string_section_header;
	Elf64_Sym		*symbole_table;
	size_t			symbole_table_size;
	size_t			name_len;
	size_t			e_shnum;
	unsigned char	*name_ptr;

	section_header = (Elf64_Shdr *)(file_info.data + header->e_shoff);
	if (header->e_shnum >= SHN_LORESERVE) {
		e_shnum = section_header->sh_size;
	}
	else {
		e_shnum = header->e_shnum;
	}

	for (size_t i = 0; i < e_shnum; i++)
	{
		if (section_header[i].sh_type == SHT_SYMTAB || 
			section_header[i].sh_type == SHT_DYNSYM )
		{
			if (section_header[i].sh_link == 0) {
				printf("nm: '%s': invalid .sh_link in section table entry %lu\n", file_info.path, i);
				return 1;
			}
			if (section_header[i].sh_entsize != sizeof(Elf64_Sym)) {
				printf("nm: '%s': invalid .sh_entsize in section table entry %lu\n", file_info.path, i);
				return 1;
			}
			if (section_header[i].sh_size % section_header[i].sh_entsize != 0) {
				printf("nm: '%s': invalid .sh_size in section table entry %lu\n", file_info.path, i);
				return 1;
			}

			symbole_table = (Elf64_Sym *)(section_header[i].sh_offset + file_info.data);
			string_section_header = section_header + section_header[i].sh_link;
			name_ptr = (unsigned char *)(string_section_header->sh_offset + file_info.data);

			symbole_table_size = section_header[i].sh_size / section_header[i].sh_entsize;
			for (size_t j = 0; j < symbole_table_size; j++)
			{
				name_len = symbole_table[j].st_name;
				while (name_ptr[name_len] != '\0')
				{
					if (name_len >= string_section_header->sh_size) {
						printf("nm: '%s': invalid .st_name in symbol table entry %lu in section table entry %lu\n", file_info.path, j, i);
						return 1;
					}
					name_len++;
				}

				unsigned char st_type = ELF64_ST_TYPE(symbole_table[j].st_info);
				unsigned char st_bind = ELF64_ST_BIND(symbole_table[j].st_info);

				if (st_type != STT_NOTYPE &&
					st_type != STT_OBJECT &&
					st_type != STT_FUNC &&
					st_type != STT_SECTION &&
					st_type != STT_FILE &&
					st_type != STT_COMMON &&
					st_type != STT_TLS &&
					st_type < STT_LOPROC )
				{
					printf("nm: '%s': invalid .st_info type in symbol table entry %lu in section table entry %lu\n", file_info.path, j, i);
					return 1;
				}

				if (st_bind != STB_LOCAL &&
					st_bind != STB_GLOBAL &&
					st_bind != STB_WEAK &&
					st_bind < STB_LOPROC )
				{
					printf("nm: '%s': invalid .st_info bind in symbol table entry %lu in section table entry %lu\n", file_info.path, j, i);
					return 1;
				}

				unsigned char st_other = ELF64_ST_VISIBILITY(symbole_table[j].st_other);
				if (st_other != STV_DEFAULT &&
					st_other != STV_INTERNAL &&
					st_other != STV_HIDDEN &&
					st_other != STV_PROTECTED )
				{
					printf("nm: '%s': invalid .st_other in symbol table entry %lu in section table entry %lu\n", file_info.path, j, i);
					return 1;
				}
			}
		}
	}
	return 0;
}


int
elf_check_x64(struct s_file_info file_info)
{
	Elf64_Ehdr	*header;
	int			err;

	header = (Elf64_Ehdr *)file_info.data;
	if (header->e_version != EV_CURRENT) {
		printf("nm: '%s': invalid version in ELF header\n", file_info.path);
		return 1;
	}
	if (header->e_ehsize != sizeof(Elf64_Ehdr)) {
		printf("nm: '%s': invalid header size in ELF header\n", file_info.path);
		return 1;
	}
	if (header->e_shstrndx != SHN_UNDEF &&
		header->e_shstrndx < SHN_LORESERVE &&
		header->e_shstrndx > header->e_shnum )
	{
		printf("nm: '%s': invalid .e_shstrndx in ELF header\n", file_info.path);
		return 1; 
	}

	err = elf_program_header_check_x64(header, file_info);
	if (err != 0) {
		return err;
	}
	err = elf_section_header_check_x64(header, file_info);
	if (err != 0) {
		return err;
	}

	err = elf_string_header_check_x64(header, file_info);
	if (err != 0) {
		return err;
	}
	err = elf_symbol_table_check_x64(header, file_info);
	if (err != 0) {
		return err;
	}

	// --TODO--
	// Relocation entries

	return err;
}