

#include <Library/UefiBootServicesTableLib.h>
#include <L1_MEMORY.h>

#include <Global/Global.h>

typedef enum
{
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_START = 0,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_START,
    ALLOCATED_INFORMATION_DOMAIN_PAGE_COUNT,
    ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_ID,
    ALLOCATED_INFORMATION_DOMAIN_MAX
}MEMORY_ALLOCATED_INFORMATION_DOMAIN;
	
#define E820_RAM        1
#define E820_RESERVED       2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE       5

//refer from EFI_MEMORY_DESCRIPTOR
typedef struct 
{
    ///
    /// Physical address of the first byte in the memory region. PhysicalStart must be
    /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
    /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
    ///
    EFI_PHYSICAL_ADDRESS  PhysicalStart;
    ///
    /// NumberOfPagesNumber of 4 KiB pages in the memory region.
    /// NumberOfPages must not be 0, and must not be any value
    /// that would represent a memory page with a start address,
    /// either physical or virtual, above 0xfffffffffffff000.
    ///
    UINT64                NumberOfPages;
    
    UINT64                FreeNumberOfPages;

    // Memory use flag page id 
    // no use: 0
    //    use: 1
    UINT8 *pMapper;
}MEMORY_CONTINUOUS;

typedef struct {
    UINT16 MemorySliceCount;
    MEMORY_CONTINUOUS MemoryContinuous[10];
    UINT64 AllocatedInformation[20][ALLOCATED_INFORMATION_DOMAIN_MAX];
    UINT16 CurrentAllocatedCount;
}MEMORY_INFORMATION;


MEMORY_INFORMATION MemoryInformation = {0};

void L2_MEMORY_CountInitial()
{
    MemoryAllocatedCurrent.AllocatedSliceCount = 0;
    MemoryAllocatedCurrent.AllocatedSizeSum = 0;
    MemoryAllocatedCurrent.AllocatedPageSum = 0;

    L1_MEMORY_SetValue((UINT8 *)&MemoryAllocatedCurrent, 0, sizeof(MemoryAllocatedCurrent));
}


EFI_STATUS L2_MEMORY_Free(UINT32 *p)
{   
    INFO_SELF(L"p: %X \r\n", p); 
    
    UINT64 PhysicalBlockStart;
    UINT64 PageStart;
    UINT64 Pages;
    UINT64 BlockID;
    UINT8 *pMapper;

    for (UINT64 i = 0; i < MemoryInformation.CurrentAllocatedCount; i++)
    {
        PhysicalBlockStart = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_START] ; // start location
        PageStart          = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PAGE_START] ; // memory block size
        Pages              = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PAGE_COUNT] ; 
        BlockID            = MemoryInformation.AllocatedInformation[i][ALLOCATED_INFORMATION_DOMAIN_PHYSICAL_BLOCK_ID] ; 

        INFO_SELF(L"PhysicalBlockStart: %X, PageStart:%llu, Pages: %llu, BlockID: %llu \r\n", PhysicalBlockStart, PageStart, Pages, BlockID); 

        if (p == PhysicalBlockStart + PageStart * 8 * 512)
        {           
            INFO_SELF(L"PhysicalBlockStart: %X, PageStart:%llu, Pages: %llu, BlockID: %llu \r\n", PhysicalBlockStart, PageStart, Pages, BlockID); 
            pMapper = MemoryInformation.MemoryContinuous[BlockID].pMapper;
            for (UINT64 j = 0; j < Pages; j++)
                pMapper[PageStart + j] = 0; 

            p = NULL;
            
            break;
        }
    }

}

float L2_MEMORY_UseRecords(UINT32 *p)
{  }

float L2_MEMORY_Remainings()
{  }


float L2_MEMORY_GETs()
{  
    EFI_STATUS                           Status;
    UINT8                                TmpMemoryMap[1];
    UINTN                                MapKey;
    UINTN                                DescriptorSize;
    UINT32                               DescriptorVersion;
    UINTN                                MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR                *MemoryMap;
    UINTN MemoryClassifySize[6] = {0};

    L1_MEMORY_SetValue(&MemoryInformation, sizeof(MemoryInformation), 0);
    
    //
    // Get System MemoryMapSize
    //
    MemoryMapSize = sizeof (TmpMemoryMap);
    Status = gBS->GetMemoryMap (&MemoryMapSize,
                                (EFI_MEMORY_DESCRIPTOR *)TmpMemoryMap,
                                &MapKey,
                                &DescriptorSize,
                                &DescriptorVersion);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);            
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    //
    // Enlarge space here, because we will allocate pool now.
    //
    MemoryMapSize += EFI_PAGE_SIZE;
    Status = gBS->AllocatePool (EfiLoaderData,
                                MemoryMapSize,
                                (VOID **) &MemoryMap);
    ASSERT_EFI_ERROR (Status);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    //
    // Get System MemoryMap
    //
    Status = gBS->GetMemoryMap (&MemoryMapSize,
                                MemoryMap,
                                &MapKey,
                                &DescriptorSize,
                                &DescriptorVersion);
    ASSERT_EFI_ERROR (Status);
    //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Status:%X \n", __LINE__, Status);
        
    //DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
    EFI_PHYSICAL_ADDRESS lastPhysicalEnd = 0;
    UINTN ContinuousMemoryPages = 0;
    UINTN ContinuousMemoryStart = 0;
    UINTN ContinuousVirtualMemoryStart = 0;
    UINTN MemoryAllSize = 0;
    UINTN E820Type = 0;

    //
    for (UINT16 Index = 0; Index < (MemoryMapSize / DescriptorSize); Index++) 
    //for (UINT16 Index = 0; Index < (MemoryMapSize / DescriptorSize) - 1; Index++)     
    //for (UINT16 Index = 0; Index < 20; Index++) 
    {
        E820Type = 0;
      //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Index:%X \n", __LINE__, Index);
        
      ////DEBUG ((EFI_D_INFO, "%d:  Status:%X \n", __LINE__, Status));
    
      if (MemoryMap->NumberOfPages == 0) 
      {
          continue;
      }
    
      switch(MemoryMap->Type) 
      {
          case EfiReservedMemoryType:
          case EfiRuntimeServicesCode:
          case EfiRuntimeServicesData:
          case EfiMemoryMappedIO:
          case EfiMemoryMappedIOPortSpace:
          case EfiPalCode:
                E820Type = E820_RESERVED;
                MemoryClassifySize[0] += MemoryMap->NumberOfPages;
                break;
                
          /// Memory in which errors have been detected.
          case EfiUnusableMemory:
                E820Type = E820_UNUSABLE;
                MemoryClassifySize[1] += MemoryMap->NumberOfPages;
                break;

           //// Memory that holds the ACPI tables.
          case EfiACPIReclaimMemory:
                E820Type = E820_ACPI;
                MemoryClassifySize[2] += MemoryMap->NumberOfPages;
                break;
        
          case EfiLoaderCode:
          case EfiLoaderData:
          case EfiBootServicesCode:
          case EfiBootServicesData:
          case EfiConventionalMemory:
                E820Type = E820_RAM; // Random access memory
                MemoryClassifySize[3] += MemoryMap->NumberOfPages;

                
                /*L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Index: %d: Start: %X Pages:%X End: %X Type: %d \n", __LINE__, 
                                                                            Index,
                                                                            MemoryMap->PhysicalStart, 
                                                                            MemoryMap->NumberOfPages,
                                                                            MemoryMap->PhysicalStart + MemoryMap->NumberOfPages * 4 * 1024,
                                                                            MemoryMap->Type);*/
                                                                            
                if (lastPhysicalEnd != MemoryMap->PhysicalStart)
               {
                    /* INFO_SELF(L"Start: %X Pages:%X End: %X\n", MemoryMap->PhysicalStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                                                                                
                     //DEBUG ((EFI_D_INFO, "%d: E820Type:%X Start:%X Virtual Start:%X Number:%X\n", __LINE__, 
                                                                                ContinuousMemoryStart,
                                                                                ContinuousVirtualMemoryStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
        
                    */

                    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].PhysicalStart = ContinuousMemoryStart;
                    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].NumberOfPages = ContinuousMemoryPages;
                    MemoryInformation.MemorySliceCount++;
                    
                    /*
                    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Start: %X  Virtual Start:%X: Pages:%X End: %X\n", __LINE__, 
                                                                                MemoryMap->PhysicalStart,
                                                                                ContinuousVirtualMemoryStart,
                                                                                ContinuousMemoryPages,
                                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                    */
                    MemoryAllSize += ContinuousMemoryPages;
                    ContinuousMemoryPages = 0;    
                    ContinuousMemoryStart = MemoryMap->PhysicalStart;
                    ContinuousVirtualMemoryStart = MemoryMap->VirtualStart;
                }
                 ContinuousMemoryPages += MemoryMap->NumberOfPages;
               lastPhysicalEnd = MemoryMap->PhysicalStart + MemoryMap->NumberOfPages * 4 * 1024;
                break;
        
          case EfiACPIMemoryNVS:       // // Address space reserved for use by the firmware.
                MemoryClassifySize[4] += MemoryMap->NumberOfPages;
                E820Type = E820_NVS;
                break;
        
          default:
                MemoryClassifySize[5] += MemoryMap->NumberOfPages;
                /*DEBUG ((DEBUG_ERROR,
                  "Invalid EFI memory descriptor type (0x%x)!\n",
                  MemoryMap->Type));
                  */  
                   //DEBUG ((EFI_D_INFO, "%d:  Invalid EFI memory descriptor type (0x%x)!\n", __LINE__, MemoryMap->Type));
                continue;         
      }

      /*//DEBUG ((EFI_D_INFO, "%d: E820Type:%X Start:%X Virtual Start:%X Number:%X\n", __LINE__, 
                                                                            E820Type, 
                                                                            MemoryMap->PhysicalStart, 
                                                                            MemoryMap->VirtualStart, 
                                                                            MemoryMap->NumberOfPages));
        */  //
        // Get next item
        //
        MemoryMap = (EFI_MEMORY_DESCRIPTOR *)((UINTN)MemoryMap + DescriptorSize);
    }
    
    MemoryAllSize += ContinuousMemoryPages;

    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].PhysicalStart = ContinuousMemoryStart;
    MemoryInformation.MemoryContinuous[MemoryInformation.MemorySliceCount].NumberOfPages = ContinuousMemoryPages;
    MemoryInformation.MemorySliceCount++;
      
    /*INFO_SELF(L"Start: %X Pages:%X End: %X\n", MemoryMap->PhysicalStart,
                                                              ContinuousMemoryPages,
                                                              ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
                                                                  
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: Start: %X: Pages:%X End: %X\n", __LINE__, 
                                                                ContinuousMemoryStart, 
                                                                ContinuousMemoryPages,
                                                                ContinuousMemoryStart + ContinuousMemoryPages * 4 * 1024);
    
    
    L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d:  (MemoryMapSize / DescriptorSize):%X MemoryClassifySize[0]:%d %d %d %d %d %d MemoryAllSize: %d\n", __LINE__,
                                                                                (MemoryMapSize / DescriptorSize),
                                                                        MemoryClassifySize[0],
                                                                        MemoryClassifySize[1],
                                                                        MemoryClassifySize[2],
                                                                        MemoryClassifySize[3],
                                                                        MemoryClassifySize[4],
                                                                        MemoryClassifySize[5],
                                                                        MemoryAllSize
                                                                        );*/
    return  MemoryClassifySize[3];                                                                    
}

EFI_STATUS L2_MEMORY_MapInitial()
{
    MEMORY_INFORMATION MemoryInformationTemp;

    MemoryInformation.CurrentAllocatedCount = 0;

    INFO_SELF(L"%X \r\n", L2_MEMORY_MapInitial);  

    L1_MEMORY_SetValue(&MemoryInformation.AllocatedInformation, sizeof(MemoryInformation.AllocatedInformation), 0);

    // Sort by pages
    for (UINT16 i = 0; i < MemoryInformation.MemorySliceCount; i++)
    {
        for (UINT16 j = 0; j < MemoryInformation.MemorySliceCount - i; j++)
        {       
            if ( MemoryInformation.MemoryContinuous[j].NumberOfPages > MemoryInformation.MemoryContinuous[j + 1].NumberOfPages)
            {
                MEMORY_CONTINUOUS temp;
                temp.NumberOfPages = MemoryInformation.MemoryContinuous[j + 1].NumberOfPages;
                temp.PhysicalStart = MemoryInformation.MemoryContinuous[j + 1].PhysicalStart;
                
                MemoryInformation.MemoryContinuous[j + 1].NumberOfPages = MemoryInformation.MemoryContinuous[j].NumberOfPages;
                MemoryInformation.MemoryContinuous[j + 1].PhysicalStart = MemoryInformation.MemoryContinuous[j].PhysicalStart;
                
                MemoryInformation.MemoryContinuous[j].NumberOfPages = temp.NumberOfPages;
                MemoryInformation.MemoryContinuous[j].PhysicalStart = temp.PhysicalStart;
            }
        }
    }

    // Initial 
    for (UINT16 i = 0; i < MemoryInformation.MemorySliceCount; i++)
    {
        UINT8 AddOne = (MemoryInformation.MemoryContinuous[i].NumberOfPages % 8 != 0) ? 1 : 0;

        // Allocate memory page use flag. for memory allocate or free.
        // Maybe use bit map, can save more memory
        MemoryInformation.MemoryContinuous[i].pMapper = AllocateZeroPool(MemoryInformation.MemoryContinuous[i].NumberOfPages);  
        if (NULL == MemoryInformation.MemoryContinuous[i].pMapper)
        {
            //L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: AllocateZeroPool failed \n", __LINE__);
        }
        UINT64 PhysicalStart = MemoryInformation.MemoryContinuous[i].PhysicalStart;
        UINT64 NumberOfPages = MemoryInformation.MemoryContinuous[i].NumberOfPages;
        MemoryInformation.MemoryContinuous[i].FreeNumberOfPages = NumberOfPages;
        
        // check sort above by pages whether success.
        L2_DEBUG_Print3(DISPLAY_LOG_ERROR_STATUS_X, DISPLAY_LOG_ERROR_STATUS_Y, WindowLayers.item[GRAPHICS_LAYER_SYSTEM_LOG_WINDOW], "%d: i: %d Start: %X Pages: %d End: %X \n", __LINE__, i, 
                                        PhysicalStart, 
                                        NumberOfPages,
                                        PhysicalStart +NumberOfPages * 4 * 1024);
                                            
    }

    // want to use Link to save Memory information
    // 1. Sort by avaliable pages number
    // 2. After allocate new memory,  the link can resort
    // 3. After free allocated memory, the link can resort
    // 4. when allocate memory, can allocate almost match with request
    // 5. maybe more and more fragment, after allocates and frees.
}


float L2_MEMORY_Initial()
{
    MemorySize = (float)L2_MEMORY_GETs();
    L2_MEMORY_MapInitial();
}
