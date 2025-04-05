#ifndef __NM_H__
# define __NM_H__

#include <stddef.h>	// size_t

struct s_file_info
{
	int			fd;
	size_t		size;
	const char	*path;
	char		*data;

	// used to check alignement of ELF program header
	size_t			page_size;

	// sould only use by mmap() & munmap()
	size_t	_map_size;
	
};

#endif /* __NM_H__ */