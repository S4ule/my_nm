
#include "nm.h"				// struct s_file_info
#include "elf_checker.h"	// elf_check_header()
#include "elf_getter.h"		// get_elf_info(), get_elf_section_header()

#include <string.h>		// strcmp(), strncmp()
#include <stdio.h>		// printf()
#include <stdlib.h>		// free(), malloc()

#include <sys/mman.h>	// mmap(), munmap()
#include <sys/stat.h>	// open()
#include <fcntl.h>		// open()
#include <unistd.h>		// close()


static int
load_file(struct s_file_info *file_info)
{
	// local scop
	{	struct stat st;
		int			err;

	err = fstat(file_info->fd, &st);
	if (err == -1) {
		printf("nm: '%s': fstat() call fail\n", file_info->path);
		return 1;
	}
	file_info->size = st.st_size; } /* end local scop */

	
	file_info->page_size = sysconf(_SC_PAGE_SIZE);
	if (file_info->page_size == 0) {
		printf("nm: '%s': sysconf(_SC_PAGE_SIZE) == 0\n", file_info->path);
		return 1;
	}
	file_info->_map_size = (file_info->size / file_info->page_size + 1) * file_info->page_size;

	file_info->data = mmap(NULL, file_info->_map_size, PROT_READ, MAP_PRIVATE, file_info->fd, 0);
	if (file_info->data == MAP_FAILED) {
		printf("nm: '%s': mmap() call fail\n", file_info->path);
		return 1;
	}
	return 0;
}

static void
unload_file(struct s_file_info *file_info)
{
	munmap(file_info->data, file_info->_map_size);
	return ;
}

struct s_formated_symbol
{
	uint64_t		value;
	unsigned char	type;
	char			*name;
};

static void
short_symbole(struct s_formated_symbol *formated_symbol, size_t size)
{
	struct s_formated_symbol copy;

	for (size_t i = 0; i + 1 < size; i++)
	{
		if (strcmp(formated_symbol[i].name, formated_symbol[i + 1].name) > 0)
		{
			copy = formated_symbol[i];
			formated_symbol[i] = formated_symbol[i + 1];
			formated_symbol[i + 1] = copy;
			i = 0;
			continue ;
		}
	}
	return ;
}

static void
print_symbole(struct s_formated_symbol *formated_symbol, size_t size)
{
	short_symbole(formated_symbol, size);

	for (size_t i = 0; i < size; i++)
	{
		if (formated_symbol[i].type == 'a' || strlen(formated_symbol[i].name) == 0)
		{
			// nm don't print this
			continue ;
		}


		if (formated_symbol[i].type == 'U' ||
			formated_symbol[i].type == 'w')
		{
			// nm don't print value for this
			printf("                 %c %s\n", formated_symbol[i].type, formated_symbol[i].name);
		}
		else
		{
			if (formated_symbol[i].value != 0)
			{
				printf("%016lx %c %s\n", formated_symbol[i].value, formated_symbol[i].type, formated_symbol[i].name);
			}
			else
			{
				printf("                 %c %s\n", formated_symbol[i].type, formated_symbol[i].name);	
			}			
		}
	}
	return ;
}

static void
print_symtab(struct s_elf_info elf_info, const char *file_path)
{
	struct s_elf_section_header elf_section_header;
	struct s_elf_symbol_header elf_symbol_header;
	struct s_formated_symbol *formated_symbol;
	int err;

	for (size_t i = 0; i < elf_info.section_header_n; i++)
	{
		get_elf_section_header(&elf_section_header, i, elf_info);
		
		// nm print this section
		if (elf_section_header.name != NULL && strncmp(elf_section_header.name, ".symtab", 8) == 0)
		{
			size_t formated_symbol_size;
			formated_symbol_size = (elf_section_header.size / elf_section_header.entry_size);
			formated_symbol = malloc(sizeof(struct s_formated_symbol) * formated_symbol_size);
			if (formated_symbol == NULL)
			{
				printf("error: malloc failed\n");
				return ;
			}

			size_t j;

			j = 0;
			while (j < formated_symbol_size)
			{
				err = get_elf_symbol_header(&elf_symbol_header, j, elf_section_header, elf_info);
				if (err == -1)
				{
					break ;
				}

				formated_symbol[j].value = elf_symbol_header.value;
				formated_symbol[j].type = get_elf_symbol_char_type(elf_symbol_header, elf_info);
				formated_symbol[j].name = elf_symbol_header.name;
				j++;
			}
			if (j != formated_symbol_size)
			{
				formated_symbol_size = j;
			}

			if (formated_symbol_size == 0)
			{
				free(formated_symbol);
				break ;
			}
			print_symbole(formated_symbol, formated_symbol_size);
			free(formated_symbol);
			return ;
		}
	}
	printf("nm: %s: no symbols\n", file_path);
	return ;
}

static int
nm(char *file_path)
{
	struct s_file_info file_info;
	int	err;

	file_info.path = file_path;
	file_info.fd = open(file_info.path, O_RDONLY);
	if (file_info.fd < 0) {
		printf("nm: '%s': No such file\n", file_info.path);
		return 1;
	}

	err = load_file(&file_info);
	close(file_info.fd);
	if (err != 0) {
		printf("error\n");
		return err;
	}

	err = elf_check(file_info);
	if (err != 0) {
		unload_file(&file_info);
		return err;
	}

	{
		struct s_elf_info elf_info;

		get_elf_info(&elf_info, file_info);
		print_symtab(elf_info, file_path);
	}

	unload_file(&file_info);
	return 0;
}

int
main(int ac, char **av)
{
	int err;

	err = 0;
	if (ac < 2) {
		err += nm("./a.out");
	}
	else
	{
		for (int i = 1; i < ac; i++)
		{
			if (ac > 2)
			{
				printf("\n%s:\n", av[i]);
			}
			err += nm(av[i]);
		}
	}
	return err;
}
