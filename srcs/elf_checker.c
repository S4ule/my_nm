
#include <stdio.h>			// printf()
#include "elf_checker.h"	// elf_check_x32(), elf_check_x64(), struct s_file_info
#include <elf.h>			// ...

int elf_check_x32(struct s_file_info file_info);
int elf_check_x64(struct s_file_info file_info);

int
elf_check(struct s_file_info file_info)
{
	unsigned char file_class;

	if (EI_NIDENT > file_info.size) {
		printf("nm: '%s': missing or incomplet header\n", file_info.path);
		return 1;
	}

	if (file_info.data[EI_MAG0] != ELFMAG0 ||
		file_info.data[EI_MAG1] != ELFMAG1 ||
		file_info.data[EI_MAG2] != ELFMAG2 ||
		file_info.data[EI_MAG3] != ELFMAG3 )
	{
		printf("nm: '%s': magic number don't match in ELF header\n", file_info.path);
		return 1;
	}

	if (file_info.data[EI_CLASS] != ELFCLASS32 &&
		file_info.data[EI_CLASS] != ELFCLASS64 )
	{
		printf("nm: '%s': invalid class in ELF header\n", file_info.path);
		return 1;
	}
	file_class = file_info.data[EI_CLASS];

	if (file_info.data[EI_DATA] != ELFDATA2LSB &&
		file_info.data[EI_DATA] != ELFDATA2MSB )
	{
		printf("nm: '%s': unknown data format in ELF header\n", file_info.path);
		return 1;
	}

	if (file_info.data[EI_VERSION] != EV_CURRENT) {
		printf("nm: '%s': invalid version in ELF header\n", file_info.path);
		return 1;
	}

	if ((file_class == ELFCLASS32 && sizeof(Elf32_Ehdr) > file_info.size) ||
		(file_class == ELFCLASS64 && sizeof(Elf64_Ehdr) > file_info.size) )
	{
		printf("nm: '%s': missing or incomplet header\n", file_info.path);
		return 1;
	}

	if (file_class == ELFCLASS32) {
		return elf_check_x32(file_info);
	}
	return elf_check_x64(file_info);
}