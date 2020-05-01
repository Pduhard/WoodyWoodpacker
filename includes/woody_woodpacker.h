#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include "../libft/libft.h"
# include "elf.h"
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <stdio.h>

# define CHECK_GOOD 1
# define CHECK_BAD  0

# define USAGE   "FILE"

typedef struct  s_data
{
  off_t         file_size;
  char          *file_name;
  char          *bin_name;
  void          *file_start;
  Elf64_Ehdr    *elf_hdr;
  Elf64_Addr    real_entry_point;
}               t_data;

/* error function */

void    ft_throw_error(char *message, ...);
int     ft_return_error(int ret, char *message, ...);
int     ft_memalloc_error(int ret, size_t size, char *prog_name);
int     ft_usage_error(int ret, char *usage, char *prog_name);

int     modify_segment_header(t_data *data);

#endif
