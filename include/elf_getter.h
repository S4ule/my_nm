#ifndef __ELF_GETTER_H__
# define __ELF_GETTER_H__

# include "nm.h"		// struct s_file_info
# include <stdint.h>	// uint

struct s_elf_program_header
{
	uint32_t	type;
	uint32_t	flag;
	uint64_t	offset;
	uint64_t	virtual_addr;
	uint64_t	physical_addr;
	uint64_t	file_size;
	uint64_t	memory_size;
	uint64_t	align;
};

struct s_elf_section_header
{
	char		*name;
	uint32_t	type;
	uint64_t	flags;
	uint64_t	addr;
	uint64_t	offset;
	uint64_t	size;
	uint32_t	linked_section_header_index;
	uint32_t	info;
	uint64_t	addr_align;
	uint64_t	entry_size;
};

struct s_elf_symbol_header
{
	char			*name;
	uint32_t		name_index;
	unsigned char 	symbol_type; // symbol's type and binding
	unsigned char	symbol_visibility;
	uint16_t		linked_section_header_index;
	uint64_t		value;
	uint64_t		size;
};

/*
struct s_elf_relocation_header
{
	uint64_t	offset;
	uint64_t	info;
	uint16_t	addend_used; // 0 if false
	int64_t		addend; // This member specifies a constant addend used to compute the value to be stored into the relocatable field.
};
*/

struct s_elf_info
{
	unsigned char elf_class; // x32 x64
	unsigned char data_encoding; // little-endian/big-endian
	unsigned char version;
	unsigned char os_abi;
	unsigned char abi_version;

	uint16_t	file_type;
	uint16_t	machine_architecture;
	uint32_t	flag; // This member holds processor-specific flags associated with the file

	uint64_t	entry_point;

	uint16_t	string_table_section_index;

	uint64_t	program_header_offset;
	uint16_t	program_header_size;
	size_t		program_header_n;

	uint64_t	section_header_offset;
	uint16_t	section_header_size;
	size_t		section_header_n;

	char		*start_of_file;
};

unsigned char get_elf_symbol_char_type(struct s_elf_symbol_header elf_symbol_header, struct s_elf_info elf_info);
int get_elf_symbol_header(struct s_elf_symbol_header *elf_symbol_header, size_t index, struct s_elf_section_header elf_section_header, struct s_elf_info elf_info);
int get_elf_section_header(struct s_elf_section_header *elf_section_header, size_t index, struct s_elf_info elf_info);
int get_elf_program_header(struct s_elf_program_header *elf_program_header, size_t index, struct s_elf_info elf_info);
void get_elf_info(struct s_elf_info *elf_info, struct s_file_info file_info);

#endif /* __ELF_GETTER_H__ */