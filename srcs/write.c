#include "woody_woodpacker.h"

// char injection[] = {
// 	// push
// 	0x9c, 0x50, 0x57, 0x56, 0x54, 0x52, 0x51,
// 	// write
// 	0xbf, 0x01, 0, 0, 0,
// 	0x48, 0x8d, 0x35, 0x0c, 0, 0, 0,
// 	0xba, 0x10, 0, 0, 0,
// 	0x48, 0x89, 0xf8,
// 	0x0f, 0x05,
// 	0xeb, 0x10,
// 	// string
//   0x2e, 0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x0a, 0x00,
// 	// pop
//   0x59, 0x5a, 0x5c, 0x5e, 0x5f, 0x58, 0x9d
// }; // need to add jmp to old entry point and gg ?
// size_t injection_size = sizeof(injection);

extern unsigned int g_unpack_len;
extern unsigned int g_unpack_offset;

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
        printf("HA\n"); // CA PASSE PAS ICI MAIS NP
         phdr[i].p_offset += g_unpack_len  + offset;
         if (phdr[i].p_offset < data->new_section->sh_offset + data->new_section->sh_size)
         {
            phdr[i].p_offset = data->new_section->sh_offset + data->new_section->sh_size;
         }
      }
      printf("New Phdr[%d],v_addr %08lx type %d filesize %lu memsize %lu align %lu offset %lx offset + size: %lx\n", i, phdr[i].p_vaddr, phdr[i].p_type, phdr[i].p_filesz, phdr[i].p_memsz, phdr[i].p_align, phdr[i].p_offset, phdr[i].p_memsz + phdr[i].p_offset);

      i++;
    }
    i = 0;
    offset = 0;
    shdr = data->new_shdr;
    char *str = (char *)(data->new_shdr[data->elf_hdr->e_shstrndx].sh_offset + data->file_start);
    while (data->new_shdr + i != data->new_section)
    {
      printf("new shdr[%d] vaddr %lx file offset: %lx : %s type %d %08lx + %lx size = %lx off\n", i, shdr[i].sh_addr, shdr[i].sh_offset, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

      // printf("new shdr[%d] %lx : %s type %d %08lx + %lx size = %lx off\n",
      // i, shdr[i].sh_addr, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset,
      // shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

        i++;
    }
      printf("new shdr[%d] vaddr %lx file offset: %lx : %s type %d %08lx + %lx size = %lx off\n", i, shdr[i].sh_addr, shdr[i].sh_offset, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

    // printf("new shdr[%d] %lx : %s type %d %08lx + %lx size = %lx off\n",
    // i, shdr[i].sh_addr, str + shdr[i].sh_name, shdr[i].sh_type, shdr[i].sh_offset,
    // shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

    i++;
    offset = data->new_section->sh_offset + data->new_section->sh_size;
    while (i < data->elf_hdr->e_shnum)
    {
      if (shdr[i].sh_offset < offset)
        shdr[i].sh_offset = offset;
      offset = shdr[i].sh_offset + shdr[i].sh_size;

    //   shdr[i].sh_offset += g_unpack_len  + offset;
      // if (shdr[i].sh_offset + shdr[i].sh_size >= data->new_section->sh_offset && shdr + i != data->new_section)
      // {
      //    if (shdr[i].sh_offset < data->new_section->sh_offset + data->new_section->sh_size)
      //    {
      //       shdr[i].sh_offset = data->new_section->sh_offset + data->new_section->sh_size;
      //    }
      // }
      printf("new shdr[%d] vaddr %lx file offset: %lx : %s al %lx %08lx + %lx size = %lx off\n", i, shdr[i].sh_addr, shdr[i].sh_offset, str + shdr[i].sh_name, shdr[i].sh_addralign, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_offset + shdr[i].sh_size);

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
    padding = ft_strnew((size_t)(pad_to - *size));
  if (write(fd, padding, pad_to - *size) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  *size = pad_to;
  free(padding);
}

void write_new_exe(t_data *data)
{
    // if (write(fd, data->file_start, data->file_size) == -1)
  //   return ;
  // return ;
  int fd;
  int size;
  int jump = (int)(data->real_entry_point - data->new_section->sh_addr - g_unpack_len + 5);
  Elf64_Shdr *old_shdr = data->file_start + data->elf_hdr->e_shoff;

  size = 0;
  if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1)
  {
    ft_throw_error("Can't create file: woody\n");
    return ;
  }
  data->pt_load_inject_in->p_memsz += g_unpack_len;
  data->pt_load_inject_in->p_filesz = data->pt_load_inject_in->p_memsz;

  data->elf_hdr->e_entry = data->new_section->sh_addr + g_unpack_offset;
  modify_offset(data);
  data->elf_hdr->e_shoff = data->new_shdr[data->elf_hdr->e_shnum - 1].sh_offset + data->new_shdr[data->elf_hdr->e_shnum - 1].sh_size;
  if (write(fd, data->elf_hdr, sizeof(Elf64_Ehdr)) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  size += sizeof(Elf64_Ehdr);
  write_padding(fd, &size, data->elf_hdr->e_phoff);
  size += data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum;
  printf(" size after phdr %x\n", size);
  if (write(fd, data->file_start + data->elf_hdr->e_phoff, data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  int i = 0;
  while (data->new_shdr + i != data->new_section)
  {
    write_padding(fd, &size, data->new_shdr[i].sh_offset);
    if (data->new_shdr + i + 1 != data->new_section) // dont write bss
    {
      if (write(fd, data->file_start + data->new_shdr[i].sh_offset, data->new_shdr[i].sh_size) == -1)
        ft_fatal_error(EXIT_FAILURE, "Write error...\n");
      size += data->new_shdr[i].sh_size;
    }
    i++;
  }
  // i++;
 // if (write(fd, data->file_start + size, data->new_section->sh_offset - size) == -1)
   // ft_fatal_error(EXIT_FAILURE, "Write error...\n");

  //exit(0);
 // size = data->new_section->sh_offset;
  // printf("unpack len %u \n", g_unpack_len);
  write_padding(fd, &size, data->new_section->sh_offset);
  write(fd, &unpack, g_unpack_len - 5);
  // exit(0);
  // if (write(fd, injection, g_unpack_len) == -1)
  //   ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  // char jmp = 0xe9;
  // if (write(fd, &jmp, 1) == -1)
  //   ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  if (write(fd, (char *)&jump, sizeof(int)) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  if (write(fd, "\xc3", 1) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n"); // ret
  i++;
  size += g_unpack_len;
  //exit(0);
  while (i < data->elf_hdr->e_shnum)
  {
    write_padding(fd, &size, data->new_shdr[i].sh_offset);
    if (write(fd, data->file_start + old_shdr[i - 1].sh_offset, data->new_shdr[i].sh_size) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");
    size += data->new_shdr[i].sh_size;
    i++;
  }
  write_padding(fd, &size, data->elf_hdr->e_shoff);
  //printf("shentsize : %hx %\n", data->elf_hdr->e_shentsize);
  if (write(fd, data->new_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum) == -1)
    ft_fatal_error(EXIT_FAILURE, "Write error...\n");
  size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
  printf("%x %x %x\n", size, size - g_unpack_len - 5, data->elf_hdr->e_shstrndx);
  printf("injected entry point: %lx\n", data->elf_hdr->e_entry);
  printf("new ph_off: %lx nb entries %d\n", data->elf_hdr->e_phoff, data->elf_hdr->e_phnum);

  printf("new_shoff: %lx nb entries %d\n", data->elf_hdr->e_shoff, data->elf_hdr->e_shnum);
  printf("jump %d real entry point %lx => %lx == %lx\n",
    jump, data->real_entry_point, data->real_entry_point - jump, data->elf_hdr->e_entry + g_unpack_len - 5);

  // printf("injected entry point: %lx\n", data->elf_hdr->e_entry);

  // printf("injected entry point: %lx\n", data->elf_hdr->e_entry);

}
/*
void zdwrite_new_exe(t_data *data)
{
    int fd;
    int size;
    int jump = (int)(data->real_entry_point - data->new_section->sh_addr - g_unpack_len);
    Elf64_Shdr  *old_shdr;

    size = 0;
    if ((fd = open("woody", O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1)
    {
      ft_throw_error("Can't open file: woody\n");
      return ;
    }
    data->pt_load_inject_in->p_memsz += g_unpack_len ;
    // printf("entry %lx injection v_addr %lx v_addr + injection size - jmp: %lx\n", data->elf_hdr->e_entry, data->new_section->sh_addr, data->new_section->sh_addr + g_unpack_len + jump);
    data->elf_hdr->e_entry = data->new_section->sh_addr;

    // data->elf_hdr->e_shstrndx += g_unpack_len ;
    modify_offset(data);
    old_shdr = (Elf64_Shdr *)(data->file_start + data->elf_hdr->e_shoff);
    data->elf_hdr->e_shoff = data->new_shdr[data->elf_hdr->e_shnum - 1].sh_offset + data->new_shdr[data->elf_hdr->e_shnum - 1].sh_size;
    // inject_code(data);
    if (write(fd, data->elf_hdr, sizeof(Elf64_Ehdr)) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");
    size += sizeof(Elf64_Ehdr);
    printf("size after elf_hdr %x\n", size);
    write_padding(fd, &size, data->elf_hdr->e_phoff);
    size += data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum;

    printf("%lx phdr off %lx sh off\n ", data->elf_hdr->e_phoff, data->elf_hdr->e_shoff);
    printf("size after ph_hdr %x (size = %x * %x)\n", size,data->elf_hdr->e_phentsize , data->elf_hdr->e_phnum);
    if (write(fd, data->file_start + data->elf_hdr->e_phoff, data->elf_hdr->e_phentsize * data->elf_hdr->e_phnum) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");

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
      if (write(fd, data->file_start + data->new_shdr[i].sh_offset, data->new_shdr[i].sh_size) == -1)
        ft_fatal_error(EXIT_FAILURE, "Write error...\n");
      size += data->new_shdr[i].sh_size;
      i++;
    }
    if (write(fd, injection, g_unpack_len) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");
    if (write(fd, "\xe9", 1) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n"); // jump
    // int jump = (int)(data->real_entry_point - data->new_section->sh_addr - g_unpack_len);

    if (write(fd, (char *)&jump, sizeof(int)) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");
    i++;
    // return;
    size += g_unpack_len ;
    // printf("size after injection %x shoff %lx\n", size, data->elf_hdr->e_shoff);
    old_shdr = data->file_start + data->elf_hdr->e_shoff - g_unpack_len - 5;
    while (i != data->elf_hdr->e_shnum)
    {
      // if ()
      // write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);

      write_padding(fd, &size, data->new_shdr[i].sh_offset);
      if (data->new_shdr[i].sh_size)
          if (write(fd, data->file_start + old_shdr[i - 1].sh_offset, data->new_shdr[i].sh_size) == -1)
            ft_fatal_error(EXIT_FAILURE, "Write error...\n");
      size += data->new_shdr[i].sh_size;
      // if (i + 1 != data->elf_hdr->e_shnum)
      i++;
    }
    // printf("size now %x\n", size);
    // write(1, (char *)&jump, sizeof(int));
    // int jump;
    // printf("%hhx\n", (char)(jump >> 8));
    // write(fd, injection, g_unpack_len);


    // return ;
    //
    // write_padding(fd, &size, data->elf_hdr->e_shoff);
    // printf("size after padding %x\n", size);
    // size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
    // printf("size after sh_hdr %x (size = %x * %x)\n", size,data->elf_hdr->e_shentsize , data->elf_hdr->e_shnum);

    if (write(fd, data->new_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum) == -1)
      ft_fatal_error(EXIT_FAILURE, "Write error...\n");
    size += data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum;
    // int i = 0;
    // printf("size atm %x added : %lx, original ?? %lx\n", size, g_unpack_len  + sizeof(Elf64_Shdr), size - (g_unpack_len  + sizeof(Elf64_Shdr)));

    // PRINT SECTION DATA AND THATS IT ??
    // while (data->new_shdr + i != data->new_section)
    // {
    //   // if ()
    //   // if (write(fd, data->file_startnew_shdr, data->elf_hdr->e_shentsize * data->elf_hdr->e_shnum);
    //
    //
    //   i++;
    // }
    //
}
*/