#include "woody_woodpacker.h"

Elf64_Shdr create_new_section(t_data *data)
{
  Elf64_Shdr  sect;

  ft_bzero(&sect, sizeof(Elf64_Shdr));
  sect.sh_type = SHT_PROGBITS;
  sect.sh_flags = (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR);
  // sect.sh_addr = ???
  // sect.sh_offset =
  (void)data;
  return (sect);
}

int modify_sections_header(t_data *data)
{
  Elf64_Shdr  *shdr;
  Elf64_Shdr  *new_shdr;
  // Elf64_Off   new_off = 0;
  Elf64_Half  i;
  char        *str_tab;

  i = 0;
  shdr = (Elf64_Shdr *)(data->file_start + data->elf_hdr->e_shoff);
  str_tab = (char *)(data->file_start + shdr[data->elf_hdr->e_shstrndx].sh_offset);
  printf("e_shstrndx : %hu entry pint addr %08llx\n", data->elf_hdr->e_shstrndx, data->real_entry_point);
  if (!(new_shdr = ft_memalloc(sizeof(Elf64_Shdr) * data->elf_hdr->e_shnum + 1)))
    return (CHECK_BAD);
  while (i < data->elf_hdr->e_shnum)
  {
    if (!strcmp(str_tab + shdr[i].sh_name, SECT_TO_ENCRYPT))
      data->encrypt = &(shdr[i]);
    new_shdr[i] = shdr[i];
    // if (shdr[i].sh_offset + sh_size)
    printf("shdr[%d] %llx : %s type %d %08llx + %llx size = %llx off\n", i, shdr[i].sh_addr, str_tab + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);
    i++;
  }
  new_shdr[i] = create_new_section(data);
  return (CHECK_GOOD);
}
