#include <efi.h>
#include <efilib.h>

#define ErrorCheck(actual, expected) \
  if(actual != expected) {\
    Print(L"Actual != Expected %s\n",#expected); \
    return actual; \
  }
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *systemTable)
{
//this check obviusly failed!!!
//#if defined(_GNU_EFI)
   InitializeLib(ImageHandle, systemTable);
//#endif

   Print(L"Hello, world!\n");
   /*
   typedef
   EFI_STATUS
   GetMemoryMap (
     IN OUT UINTN                 *MemoryMapSize,
     IN OUT EFI_MEMORY_DESCRIPTOR *MemoryMap,
     OUT    UINTN                 *MapKey,
     OUT    UINTN                 *DescriptorSize,
     OUT    UINT32                *DescriptorVersion
   );*/
  /* UINTN mapsize;
   EFI_MEMORY_DESCRIPTOR map;
   UINTN key;
   UINTN desc_size;
   UINT32 desc_version;
   if (GetMemoryMap(&mapsize,&map,&key,&desc_size,&desc_version) != EFI_SUCCESS)
   {
     Print(L"Failed to get memory map!\n");
   }
   else
   {
     Print(L"Found memory map size %08x\n",mapsize);
   }*/

  EFI_STATUS result;


  UINTN mapSize = 0, mapKey, descriptorSize;
  EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
  UINT32 descriptorVersion;
  // Get the required memory pool size for the memory map...
  result = uefi_call_wrapper((void *)systemTable->BootServices->GetMemoryMap, 5, &mapSize, &memoryMap, NULL, &descriptorSize, NULL);
  ErrorCheck(result, EFI_BUFFER_TOO_SMALL);

  // Allocating the pool creates at least one new descriptor... for the chunk of memory changed to EfiLoaderData
  // Not sure that UEFI firmware must allocate on a memory type boundry... if not, then two descriptors might be created
  mapSize += 2 * descriptorSize;

  // Get a pool of memory to hold the map...
  result = uefi_call_wrapper((void *)systemTable->BootServices->AllocatePool, 3, EfiLoaderData, mapSize, (void **)&memoryMap);
  ErrorCheck(result, EFI_SUCCESS);
  // Get the actual memory map...
  result = uefi_call_wrapper((void *)systemTable->BootServices->GetMemoryMap, 5, &mapSize, &memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
  ErrorCheck(result, EFI_SUCCESS);

  Print(L"Found memory map size %08x\n",mapSize);

/*
  result = uefi_call_wrapper((void *)systemTable->BootServices->ExitBootServices, 2, imageHandle, mapKey);
  ErrorCheck(result, EFI_SUCCESS);
*/

  // TODO: Boot Services no longer available. Do whatever with Runtime Services... (ex: start OS kernal executable)

   return EFI_SUCCESS;
}
