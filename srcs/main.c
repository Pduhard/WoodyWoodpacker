#include "woody_woodpacker.h"


void *mmap_file(char *bin_name, char *file_name, off_t *file_size)
{
  int         fd;
  struct stat stat_buf;
  void        *mapped_file;

  mapped_file = NULL;
  if ((fd = open(file_name, O_RDONLY)) < 0 || fstat(fd, &stat_buf) < 0)
  {
    dprintf(2, "error: %s: can't open file: %s\n", bin_name, file_name);
    return (NULL);
  }
  *file_size = stat_buf.st_size;
  if (!*file_size)
  {
    dprintf(2, "error: %s: empty file: %s\n", bin_name, file_name);
  }
  if (*file_size && (mapped_file = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
  {
    dprintf(2, "error: %s: can't map file: %s\n", bin_name, file_name);
    return (NULL);
  }
  close(fd);
  return (mapped_file);
}

int check_file_info(void *file, t_data *data)
{
  // Elf64_Ehdr    *elf_hdr;
  int           class;
  int           byte_order;

  data->elf_hdr = (Elf64_Ehdr *)file;
  if (ft_strncmp((char *)data->elf_hdr, ELFMAG, SELFMAG))
    return (ft_return_error(CHECK_BAD, "error: %s: %s: Not an elf file\n", data->bin_name, data->file_name));
  class = ((char *)data->elf_hdr)[EI_CLASS];
  byte_order = ((char *)data->elf_hdr)[EI_DATA];
  if (class != ELFCLASS64)
    return (ft_return_error(CHECK_BAD, "error: %s: %s: File architecture not suported. x86_64 only\n", data->bin_name, data->file_name));
  data->real_entry_point = data->elf_hdr->e_entry;
  data->file_start = file;
  printf("byte order %d\n", byte_order);
  return (1);
}

int main(int argc, char **argv)
{
  void    *file;
  t_data  data;

  ft_bzero(&data, sizeof(t_data));
  if (argc != 2)
    return (ft_usage_error(EXIT_FAILURE, USAGE, argv[0]));
  data.bin_name = argv[0];
  data.file_name = argv[1];
  if (!(file = mmap_file(data.bin_name, data.file_name, &data.file_size)) || check_file_info(file, &data) == CHECK_BAD)
    return (EXIT_FAILURE);
  if (!modify_segments_header(&data) || !modify_sections_header(&data))
    return (EXIT_FAILURE);
  write_new_exe(&data);
 // ft_throw_error("salut %d\n", 10);
 // printf("%d %s\n", argc, argv[0]);
  return (EXIT_SUCCESS);
}
