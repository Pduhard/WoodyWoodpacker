#ifndef WOODY_WOODPACKER_H
# define WOODY_WOODPACKER_H

# include "../libft/libft.h"
# if defined(__linux)
#  include <elf.h>
# else
#  include "mach_elf.h"
# endif
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <stdio.h>

# define CHECK_GOOD 1
# define CHECK_BAD  0

# define SECT_TO_ENCRYPT ".text"

# define USAGE   "FILE"

typedef struct  s_data
{
  off_t         file_size;
  char          *file_name;
  char          *bin_name;
  void          *file_start;
  void          *injection_start;
  Elf64_Ehdr    *elf_hdr;
  Elf64_Shdr    *new_shdr;
  Elf64_Phdr    *pt_load_inject_in;
  Elf64_Shdr    *new_section;
  Elf64_Shdr    *encrypt;
  Elf64_Addr    real_entry_point;
}               t_data;

/* error function */

// void    unpack(void);
void    ft_throw_error(char *message, ...);
int     ft_return_error(int ret, char *message, ...);
int     ft_memalloc_error(int ret, size_t size, char *prog_name);
int     ft_usage_error(int ret, char *usage, char *prog_name);
void    ft_fatal_error(int exit_status, char *message, ...);
int     modify_segments_header(t_data *data);
int     modify_sections_header(t_data *data);

void write_new_exe(t_data *data);
void unpack();
#endif
