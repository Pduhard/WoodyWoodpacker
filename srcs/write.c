#include "woody_woodpacker.h"

extern int injection_size;

void modify_offset(t_data *data)
{
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    int        i;
    int        offset;

    phdr = (Elf64_Phdr *)(data->file_start + data->elf_hdr->e_phoff);
    i = 0;
    offset = 0;
    while (i < data->elf_hdr->e_phnum)
    {
      if (phdr[i].p_offset > data->new_section->sh_offset)
      {
         phdr[i].p_offset += injection_size + 5 + offset;
         if (phdr[i].p_offset < data->new_section->sh_offset + data->new_section->sh_size)
         {
            phdr[i].p_offset = data->new_section->sh_offset + data->new_section->sh_size;
         }
      }
      printf("New Phdr[%d],v_addr %08llx type %d filesize %llu memsize %llu align %llu offset %llx offset + size: %llx\n", i, phdr[i].p_vaddr, phdr[i].p_type, phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align, phdr[i].p_offset, phdr[i].p_memsz + phdr[i].p_offset);

      i++;
    }
    i = 0;
    offset = 0;
    shdr = data->new_shdr;
    char *str = (char *)(data->new_shdr[data->elf_hdr->e_shstrndx].sh_offset + data->file_start);
    while (i < data->elf_hdr->e_shnum)
    {
      if (shdr[i].sh_offset + shdr[i].sh_size >= data->new_section->sh_offset && shdr + i != data->new_section)
      {
         shdr[i].sh_offset += injection_size + 5 + offset;
         if (shdr[i].sh_offset < data->new_section->sh_offset + data->new_section->sh_size)
         {
            shdr[i].sh_offset = data->new_section->sh_offset + data->new_section->sh_size;
         }
      }
      printf("new shdr[%d] %llx : %s type %d %08llx + %llx size = %llx off\n", i, shdr[i].sh_addr, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

      i++;
    }
}

// void inject_code(t_data *data)
// {
//     void *inject_point;
//
//     // data->elf_hdr->e_shnum++
//     // inject_point = data->file_start + data->new_section->
// }

void write_padding(int fd, int *size, int pad_to)
{
  char *padding;

  if (*size == pad_to)
    return ;
  else
    padding = ft_strnew(pad_to - *size);
  write(fd, padding, pad_to - *size);
  free(padding);
}

void write_new_exe(t_data *data)
{
    int fd;
    int size;

    size = 0;
    if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1)
    {
      ft_throw_error("Can't open file: woody\n");
      return ;
    }
    data->pt_load_inject_in->p_memsz += injection_size + 5;

    data->elf_hdr->e_shstrndx += injection_size + 5;
    modify_offset(data);
    // inject_code(data);
    write(fd, data->elf_hdr, sizeof(Elf64_Ehdr));
    size += sizeof(Elf64_Ehdr);
    write_padding(fd, &size, data->elf_hdr->e_phoff);
    size += data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum;
    write(fd, data->file_start + data->elf_hdr->e_phoff, data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum);
    write_padding(fd, &size, data->elf_hdr->e_shoff);
    size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
    write(fd, data->new_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
    // int i = 0;

    // PRINT SECTION DATA AND THATS IT ??
    // while (data->new_shdr + i != data->new_section)
    // {
    //   write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
    //
    //
    //   i++;
    // }
    //
}
