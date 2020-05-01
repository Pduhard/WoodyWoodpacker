#include "woody_woodpacker.h"

int modify_segment_header(t_data *data)
{
  Elf64_Phdr  *phdr;
  Elf64_Half  i;

  i = 0;
  phdr = (Elf64_Phdr *)(data->file_start + data->elf_hdr->e_phoff);
  while (i < data->elf_hdr->e_phnum)
  {
    printf(" Phdr[%d], type %d\n", i, phdr[i].p_type);
    i++;
  }
  return (1);
}
