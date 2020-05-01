#include "woody_woodpacker.h"

int modify_segments_header(t_data *data)
{
  Elf64_Phdr  *phdr;
  Elf64_Half  i;

  i = 0;
  phdr = (Elf64_Phdr *)(data->file_start + data->elf_hdr->e_phoff);
  while (i < data->elf_hdr->e_phnum)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      phdr[i].p_flags = (PF_R | PF_W | PF_X);

    }
    printf(" Phdr[%d],v_addr %08llx type %d filesize %llu memsize %llu align %llu offset %llx offset + size: %llx\n", i, phdr[i].p_vaddr, phdr[i].p_type, phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align, phdr[i].p_offset, phdr[i].p_filesz + phdr[i].p_offset);

    i++;
  }
  return (1);
}
