#include "file_ex.h"

size_t GetFileSize(FILE* fp) {
  fseek(fp, 0, SEEK_END);
  size_t fileSize = ftell(fp);
  rewind(fp);
  return fileSize;
}

File_t Open(const char* __restrict__ filename, const char* __restrict__ modes) {
  File_t f;
  f.ptr = fopen(filename, modes);
  f.size = 0;

  if (f.ptr != NULL) {
    f.size = GetFileSize(f.ptr);
  }

  return f;
}

SizedPtr_t ReadToEnd(const char *__restrict__ filename, const char *__restrict__ modes) {
  SizedPtr_t fb;
  File_t ft = Open(filename, modes);

  if (ft.ptr == NULL) {
    fb.data = NULL;
    fb.size = 0;
    fclose(ft.ptr);
    return fb;
  }

  fb.size = ft.size;
  fb.data = malloc(fb.size);
  fread(fb.data, fb.size, 1, ft.ptr);
  fclose(ft.ptr);

  return fb;
}
