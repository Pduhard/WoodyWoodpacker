#include "woody_woodpacker.h"

extern unsigned int g_unpack_len;
// extern char injection[];

Elf64_Shdr create_new_section(t_data *data, Elf64_Shdr *p_sect, Elf64_Off file_offset)
{
  Elf64_Shdr  sect;

  ft_bzero(&sect, sizeof(Elf64_Shdr));
  sect.sh_name = 0;
  sect.sh_type = SHT_PROGBITS;
  sect.sh_flags = (SHF_WRITE | SHF_ALLOC | SHF_EXECINSTR);
  sect.sh_addr = p_sect ? p_sect->sh_addr + p_sect->sh_size : 0;
  // sect.sh_offset = p_sect ? p_sect->v_addr + p_sect->sh_size : 0;
  sect.sh_offset = file_offset;
  sect.sh_size = g_unpack_len + 5;
  (void)data;
  return (sect);
}

int modify_sections_header(t_data *data)
{
  Elf64_Shdr  *shdr;
  Elf64_Shdr  *new_shdr;
  int         last_sect;
  // Elf64_Off   new_off = 0;
  Elf64_Half  i;
  Elf64_Off   end_off_iseg;
  char        *str_tab;

  i = 0;
  shdr = (Elf64_Shdr *)(data->file_start + data->elf_hdr->e_shoff);
  printf("sh off %lx %x e_shstrndx\n ", data->elf_hdr->e_shoff, data->elf_hdr->e_shstrndx);
  str_tab = (char *)(data->file_start + shdr[data->elf_hdr->e_shstrndx].sh_offset);
  printf("e_shstrndx : %hu entry pint addr %08lx\n", data->elf_hdr->e_shstrndx, data->real_entry_point);
  if (!(new_shdr = ft_memalloc(sizeof(Elf64_Shdr) * (data->elf_hdr->e_shnum + 1))))
    return (CHECK_BAD);
  last_sect = -1;
  end_off_iseg = data->pt_load_inject_in->p_offset + data->pt_load_inject_in->p_memsz;
  printf("end_off_iseg %lx\n", end_off_iseg);
  int check_shstrndx = 0;
  while (i < data->elf_hdr->e_shnum)
  {
    printf("ah %lx\n", shdr[i].sh_offset);
    // if (!strcmp(str_tab + shdr[i].sh_name, SECT_TO_ENCRYPT))
    //   data->encrypt = &(shdr[i]);
    if ((long long)(end_off_iseg - shdr[i].sh_offset - shdr[i].sh_size) >= 0)// before injection, Check for last section
    {
      if (last_sect < 0)
        last_sect = i;
      else if (end_off_iseg - shdr[i].sh_offset < end_off_iseg - shdr[last_sect].sh_offset)
        last_sect = i;
      new_shdr[i] = shdr[i];
    }
    else
    {
      if (i == data->elf_hdr->e_shstrndx)
        check_shstrndx = 1;
      // printf("ha\n");
      // shdr[i].sh_offset += g_unpack_len + 5; // 5 =
      new_shdr[i + 1] = shdr[i];
    }

    // if (shdr[i].sh_offset + sh_size)
    printf("shdr[%d] %lx : %s type %d %08lx + %lx size = %lx off\n", i, shdr[i].sh_addr, str_tab + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);
    i++;
  }
  if (check_shstrndx)
    data->elf_hdr->e_shstrndx++;
  if (last_sect < 1)
    return (CHECK_BAD);
  printf("insertion after : %s section \n", str_tab + shdr[last_sect].sh_name);
  new_shdr[last_sect + 1] = create_new_section(data, &(shdr[last_sect]), end_off_iseg);
  i = 0;
  while (i < data->elf_hdr->e_shnum + 1)
  {
    // printf("shdr[%d] %lx : %s type %d %08lx + %lx size = %lx off\n", i, new_shdr[i].sh_addr, str_tab + new_shdr[i].sh_name, new_shdr[i].sh_type, new_shdr[i].sh_offset, new_shdr[i].sh_size, new_shdr[i].sh_offset + new_shdr[i].sh_size);
    i++;
  }
  data->elf_hdr->e_shnum++;
  data->new_shdr = new_shdr;
  data->new_section = &(new_shdr[last_sect + 1]);
  return (CHECK_GOOD);
}
