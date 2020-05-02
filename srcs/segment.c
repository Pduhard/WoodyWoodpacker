#include "woody_woodpacker.h"
int addp(int a, int b)
{
  return (a + b);
}

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
      if(!data->pt_load_inject_in)
        data->pt_load_inject_in = &(phdr[i]);
      else if (phdr[i].p_vaddr > data->pt_load_inject_in->p_vaddr)
        data->pt_load_inject_in = &(phdr[i]);
      phdr[i].p_flags = (PF_R | PF_W | PF_X);

    }
    printf(" Phdr[%d],v_addr %08llx type %d filesize %llu memsize %llu align %llu offset %llx offset + size: %llx\n", i, phdr[i].p_vaddr, phdr[i].p_type, phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align, phdr[i].p_offset, phdr[i].p_memsz + phdr[i].p_offset);
    i++;
  }
  if (!data->pt_load_inject_in)
    return (CHECK_BAD);
  printf("%llx\n", data->pt_load_inject_in->p_offset);
  // unpack();
  // write(1, )
  return (CHECK_GOOD);
}
