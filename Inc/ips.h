#ifndef IPS_H
#define IPS_H

#include "common.h"
#include "file_ex.h"

#define IPS_HEADER ("PATCH")
#define IPS_EOF    ("EOF")

typedef struct IPS_Record_t {
  uint32_t offset;
  uint16_t size;
  uint8_t* data;

  struct IPS_Record_t* next;
} IPS_Record_t;

bool IPS_HeaderValid(const SizedPtr_t* patch);

bool IPS_ReadRecord(const SizedPtr_t* patch, IPS_Record_t* record, size_t* offset);
size_t IPS_ReadRecords(const SizedPtr_t* patch, IPS_Record_t* record, size_t* offset, size_t* lastWrittenOffset);
void IPS_FreeRecord(IPS_Record_t* record);

#endif
