#include "woody_woodpacker.h"

char injection[] = {
	// push
	0x9c, 0x50, 0x57, 0x56, 0x54, 0x52, 0x51,
	// write
	0xbf, 0x01, 0, 0, 0,
	0x48, 0x8d, 0x35, 0x0c, 0, 0, 0,
	0xba, 0x10, 0, 0, 0,
	0x48, 0x89, 0xf8,
	0x0f, 0x05,
	0xeb, 0x10,
	// string
  0x2e, 0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x0a, 0x00,
	// pop
  0x59, 0x5a, 0x5c, 0x5e, 0x5f, 0x58, 0x9d
}; // need to add jmp to old entry point and gg ?
size_t injection_size = sizeof(injection);
void modify_offset(t_data *data)
{
    Elf64_Phdr *phdr;
    Elf64_Shdr *shdr;
    int        i;
    Elf64_Off  offset;

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
    while (data->new_shdr + i != data->new_section) {
      printf("new shdr[%d] %llx : %s type %d %08llx + %llx size = %llx off\n", i, shdr[i].sh_addr, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

        i++;
    }
    printf("new shdr[%d] %llx : %s type %d %08llx + %llx size = %llx off\n", i, shdr[i].sh_addr, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

    i++;
    offset = data->new_section->sh_offset + data->new_section->sh_size;
    while (i < data->elf_hdr->e_shnum)
    {
      if (shdr[i].sh_offset < offset)
        shdr[i].sh_offset = offset;
      offset = shdr[i].sh_offset + shdr[i].sh_size;

    //   shdr[i].sh_offset += injection_size + 5 + offset;
      // if (shdr[i].sh_offset + shdr[i].sh_size >= data->new_section->sh_offset && shdr + i != data->new_section)
      // {
      //    if (shdr[i].sh_offset < data->new_section->sh_offset + data->new_section->sh_size)
      //    {
      //       shdr[i].sh_offset = data->new_section->sh_offset + data->new_section->sh_size;
      //    }
      // }
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

  printf("%x == %x ?. \n", *size, pad_to);
  if (*size >= pad_to)
    return ;
  else
    padding = ft_strnew(pad_to - *size);
  write(fd, padding, pad_to - *size);
  *size = pad_to;
  free(padding);
}

void write_new_exe(t_data *data)
{
  int fd;
  int size;
  int jump = (int)(data->real_entry_point - data->new_section->sh_addr - injection_size);
  Elf64_Shdr *old_shdr = data->file_start + data->elf_hdr->e_shoff;

  size = 0;
  if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1)
  {
    ft_throw_error("Can't open file: woody\n");
    return ;
  }
  data->pt_load_inject_in->p_memsz += injection_size + 5;
  data->elf_hdr->e_entry = data->new_section->sh_addr;
  modify_offset(data);
  data->elf_hdr->e_shoff = data->new_shdr[data->elf_hdr->e_shnum - 1].sh_offset + data->new_shdr[data->elf_hdr->e_shnum - 1].sh_size;
  write(fd, data->elf_hdr, sizeof(Elf64_Ehdr));
  size += sizeof(Elf64_Ehdr);
  write_padding(fd, &size, data->elf_hdr->e_phoff);
  size += data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum;
  printf(" size after phdr %x\n", size);
  write(fd, data->file_start + data->elf_hdr->e_phoff, data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum);
  int i = 0;
  while (data->new_shdr + i != data->new_section)
  {
    write_padding(fd, &size, data->new_shdr[i].sh_offset);
    write(fd, data->file_start + data->new_shdr[i].sh_offset, data->new_shdr[i].sh_size);
    size += data->new_shdr[i].sh_size;
    i++;
  }
  write_padding(fd, &size, data->new_section->sh_offset);
  write(fd, injection, injection_size);
  char jmp = 0xe9;
  write(fd, &jmp, 1);
  write(fd, ((char *)&jump) + 3, 1);
  write(fd, ((char *)&jump) + 2, 1);
  write(fd, ((char *)&jump) + 1, 1);
  write(fd, ((char *)&jump), 1);
  i++;
  size += injection_size + 5;
  while (i < data->elf_hdr->e_shnum)
  {
    write_padding(fd, &size, data->new_shdr[i].sh_offset);
    write(fd, data->file_start + old_shdr[i - 1].sh_offset, data->new_shdr[i].sh_size);
    size += data->new_shdr[i].sh_size;
    i++;
  }
  write_padding(fd, &size, data->elf_hdr->e_shoff);
  write(fd, data->new_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
  size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;

}

void zdwrite_new_exe(t_data *data)
{
    int fd;
    int size;
    int jump = (int)(data->real_entry_point - data->new_section->sh_addr - injection_size);
    Elf64_Shdr  *old_shdr;

    size = 0;
    if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1)
    {
      ft_throw_error("Can't open file: woody\n");
      return ;
    }
    data->pt_load_inject_in->p_memsz += injection_size + 5;
    // printf("entry %llx injection v_addr %llx v_addr + injection size - jmp: %llx\n", data->elf_hdr->e_entry, data->new_section->sh_addr, data->new_section->sh_addr + injection_size + jump);
    data->elf_hdr->e_entry = data->new_section->sh_addr;

    // data->elf_hdr->e_shstrndx += injection_size + 5;
    modify_offset(data);
    old_shdr = (Elf64_Shdr *)(data->file_start + data->elf_hdr->e_shoff);
    data->elf_hdr->e_shoff = data->new_shdr[data->elf_hdr->e_shnum - 1].sh_offset + data->new_shdr[data->elf_hdr->e_shnum - 1].sh_size;
    // inject_code(data);
    write(fd, data->elf_hdr, sizeof(Elf64_Ehdr));
    size += sizeof(Elf64_Ehdr);
    printf("size after elf_hdr %x\n", size);
    write_padding(fd, &size, data->elf_hdr->e_phoff);
    size += data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum;

    printf("%llx phdr off %llx sh off\n ", data->elf_hdr->e_phoff, data->elf_hdr->e_shoff);
    printf("size after ph_hdr %x (size = %x * %x)\n", size,data->elf_hdr->e_phentsize , data->elf_hdr->e_phnum);
    write(fd, data->file_start + data->elf_hdr->e_phoff, data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum);

    // CONTENT
    int i = 0;
    printf("size atm %x\n", size);

    // PRINT SECTION DATA AND THATS IT ??
    while (data->new_shdr + i != data->new_section)
    {
      // if ()
      // write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);

      write_padding(fd, &size, data->new_shdr[i].sh_offset);
      // if (data->new_shdr[i].sh_size && data->new_shdr[i].sh_offset + data->new_shdr[i].sh_size < data->new_section->sh_offset)
      write(fd, data->file_start + data->new_shdr[i].sh_offset, data->new_shdr[i].sh_size);
      size += data->new_shdr[i].sh_size;
      i++;
    }
    write(fd, injection, injection_size);
    write(fd, "\xe9", 1); // jump
    // int jump = (int)(data->real_entry_point - data->new_section->sh_addr - injection_size);

    write(fd, (char *)&jump, sizeof(int));
    i++;
    // return;
    size += injection_size + 5;
    // printf("size after injection %x shoff %llx\n", size, data->elf_hdr->e_shoff);
    old_shdr = data->file_start + data->elf_hdr->e_shoff - injection_size - 5;
    while (i != data->elf_hdr->e_shnum)
    {
      // if ()
      // write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);

      write_padding(fd, &size, data->new_shdr[i].sh_offset);
      if (data->new_shdr[i].sh_size)
          write(fd, data->file_start + old_shdr[i - 1].sh_offset, data->new_shdr[i].sh_size);
      size += data->new_shdr[i].sh_size;
      // if (i + 1 != data->elf_hdr->e_shnum)
      i++;
    }
    // printf("size now %x\n", size);
    // write(1, (char *)&jump, sizeof(int));
    // int jump;
    // printf("%hhx\n", (char)(jump >> 8));
    // write(fd, injection, injection_size);


    // return ;
    //
    // write_padding(fd, &size, data->elf_hdr->e_shoff);
    // printf("size after padding %x\n", size);
    // size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
    // printf("size after sh_hdr %x (size = %x * %x)\n", size,data->elf_hdr->e_shentsize , data->elf_hdr->e_shnum);

    write(fd, data->new_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
    size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
    // int i = 0;
    // printf("size atm %x added : %lx, original ?? %lx\n", size, injection_size + 5 + sizeof(Elf64_Shdr), size - (injection_size + 5 + sizeof(Elf64_Shdr)));

    // PRINT SECTION DATA AND THATS IT ??
    // while (data->new_shdr + i != data->new_section)
    // {
    //   // if ()
    //   // write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
    //
    //
    //   i++;
    // }
    //
}
