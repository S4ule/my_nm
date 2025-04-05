
#include "elf_getter.h" // all struct
#include <elf.h>	// ...

unsigned char get_elf_symbol_char_type_x64(struct s_elf_symbol_header elf_symbol_header, struct s_elf_info elf_info);
int get_elf_symbol_header_x64(struct s_elf_symbol_header *elf_symbol_header, size_t index, struct s_elf_section_header elf_section_header, struct s_elf_info elf_info);
int get_elf_section_header_x64(struct s_elf_section_header *elf_section_header, size_t index, struct s_elf_info elf_info);
int get_elf_program_header_x64(struct s_elf_program_header *elf_program_header, size_t index, struct s_elf_info elf_info);
void get_elf_info_x64(struct s_elf_info *elf_info);

unsigned char get_elf_symbol_char_type_x32(struct s_elf_symbol_header elf_symbol_header, struct s_elf_info elf_info);
int get_elf_symbol_header_x32(struct s_elf_symbol_header *elf_symbol_header, size_t index, struct s_elf_section_header elf_section_header, struct s_elf_info elf_info);
int get_elf_section_header_x32(struct s_elf_section_header *elf_section_header, size_t index, struct s_elf_info elf_info);
int get_elf_program_header_x32(struct s_elf_program_header *elf_program_header, size_t index, struct s_elf_info elf_info);
void get_elf_info_x32(struct s_elf_info *elf_info);


unsigned char
get_elf_symbol_char_type(struct s_elf_symbol_header elf_symbol_header, struct s_elf_info elf_info)
{
	if (elf_info.elf_class == ELFCLASS64) {
		return get_elf_symbol_char_type_x64(elf_symbol_header, elf_info);
	}
	return get_elf_symbol_char_type_x32(elf_symbol_header, elf_info);
}

int
get_elf_symbol_header(struct s_elf_symbol_header *elf_symbol_header, size_t index,
	struct s_elf_section_header elf_section_header, struct s_elf_info elf_info)
{
	if (elf_info.elf_class == ELFCLASS64) {
		return get_elf_symbol_header_x64(elf_symbol_header, index, elf_section_header, elf_info);
	}
	return get_elf_symbol_header_x32(elf_symbol_header, index, elf_section_header, elf_info);
}

int
get_elf_section_header(struct s_elf_section_header *elf_section_header, size_t index, struct s_elf_info elf_info)
{
	if (elf_info.elf_class == ELFCLASS64) {
		return get_elf_section_header_x64(elf_section_header, index, elf_info);
	}
	return get_elf_section_header_x32(elf_section_header, index, elf_info);
}

int
get_elf_program_header(struct s_elf_program_header *elf_program_header, size_t index, struct s_elf_info elf_info)
{
	if (elf_info.elf_class == ELFCLASS64) {
		return get_elf_program_header_x64(elf_program_header, index, elf_info);
	}
	return get_elf_program_header_x32(elf_program_header, index, elf_info);
}

void
get_elf_info(struct s_elf_info *elf_info, struct s_file_info file_info)
{
	elf_info->start_of_file = file_info.data;
	if (file_info.data[EI_CLASS] == ELFCLASS64) {
		get_elf_info_x64(elf_info);
	}
	else {
		get_elf_info_x32(elf_info);
	}
}
