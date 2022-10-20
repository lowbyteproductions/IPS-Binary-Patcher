#include "ips.h"
#include "file_ex.h"

#define PATCH_FILE  ("patches/return-to-dinosaur-land.ips")
#define ROM_FILE    ("roms/Super Mario World (U) [!].smc")
#define TARGET_FILE ("roms/patched.smc")

int main() {
  SizedPtr_t patch = ReadToEnd(PATCH_FILE, "rb");
  SizedPtr_t rom = ReadToEnd(ROM_FILE, "rb");

  printf("Opened patch file (%d bytes)\n", patch.size);
  printf("Opened rom file (%d bytes)\n", rom.size);

  if (!IPS_HeaderValid(&patch)) {
    printf("Header invalid, exiting.\n");
    return 1;
  }
  printf("Read a valid header\n");

  size_t offset = 5;
  size_t lastWrittenOffset = 0;
  IPS_Record_t* record = malloc(sizeof(IPS_Record_t));
  size_t recordsRead = IPS_ReadRecords(&patch, record, &offset, &lastWrittenOffset);
  printf("Read %d records from the patch file\n", recordsRead);


  size_t targetSize = rom.size;
  if (lastWrittenOffset + 1 > rom.size) {
    targetSize = lastWrittenOffset + 1;
  }
  printf("Creating a patched rom of size %d bytes\n", targetSize);

  uint8_t* target = malloc(targetSize);
  memcpy(target, rom.data, rom.size);

  IPS_Record_t* currentRecord = record;
  while (currentRecord) {
    memcpy(target + currentRecord->offset, currentRecord->data, currentRecord->size);
    currentRecord = currentRecord->next;
  }
  printf("Applied patches\n");

  FILE* fp = fopen(TARGET_FILE, "wb");
  fwrite(target, targetSize, 1, fp);

  printf("Wrote to target file %s\n", TARGET_FILE);
  printf("Cleaning up\n");

  // Cleanup
  fclose(fp);
  free(patch.data);
  free(rom.data);
  free(target);
  IPS_FreeRecord(record);

  printf("Done\n");

  return 0;
}
