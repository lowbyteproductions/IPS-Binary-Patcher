#include "ips.h"

bool IPS_HeaderValid(const SizedPtr_t* patch) {
  if (patch->size < 5) {
    return false;
  }

  return strncmp(patch->data, IPS_HEADER, 5) == 0;
}

bool IPS_IsEOF(const SizedPtr_t* patch, size_t* offset) {
  if (*offset + 3 > patch->size) {
    return true;
  }
  return strncmp(patch->data + *offset, IPS_EOF, 3) == 0;
}

bool IPS_ReadRecord(const SizedPtr_t* patch, IPS_Record_t* record, size_t* offset) {
  if (IPS_IsEOF(patch, offset)) return false;
  if (*offset + 3 > patch->size) return false;

  record->offset =  patch->data[(*offset)++] << 16;
  record->offset |= patch->data[(*offset)++] << 8;
  record->offset |= patch->data[(*offset)++];

  if (*offset + 2 > patch->size) return false;
  record->size =  patch->data[(*offset)++] << 8;
  record->size |= patch->data[(*offset)++];

  bool isRLE = record->size == 0;
  if (isRLE) {
    if (*offset + 2 > patch->size) return false;
    record->size =  patch->data[(*offset)++] << 8;
    record->size |= patch->data[(*offset)++];
  }

  // Set the data in the buffer
  record->data = malloc(record->size);
  if (isRLE) {
    if (*offset > patch->size) return false;
    uint8_t byte = patch->data[(*offset)++];
    memset(record->data, byte, record->size);
  } else {
    if (*offset + record->size > patch->size) return false;
    memcpy(record->data, (patch->data + *offset), record->size);
    *offset += record->size;
  }

  record->next = NULL;

  return true;
}

size_t IPS_ReadRecords(const SizedPtr_t* patch, IPS_Record_t* record, size_t* offset, size_t* lastWrittenOffset) {
  size_t recordsRead = 0;

  IPS_Record_t* prevRecord = NULL;
  IPS_Record_t* newRecord = record;
  while (IPS_ReadRecord(patch, newRecord, offset)) {
    recordsRead++;
    if (prevRecord) {
      prevRecord->next = newRecord;
    }
    prevRecord = newRecord;
    newRecord = malloc(sizeof(IPS_Record_t));
  }

  if (recordsRead >= 1) {
    free(newRecord);
  }

  *lastWrittenOffset = prevRecord->offset + prevRecord->size;

  return recordsRead;
}

void IPS_FreeRecord(IPS_Record_t* record) {
  IPS_Record_t* temp = NULL;

  while (record) {
    free(record->data);
    temp = record->next;
    free(record);
    record = temp;
  }
}
