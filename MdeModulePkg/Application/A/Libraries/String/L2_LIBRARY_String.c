#include <Library/PrintLib.h>


#include <L2_LIBRARY_String.h>

#include <Global/Global.h>

#include <Graphics/L1_GRAPHICS.h>


VOID L2_STRING_Maker (UINT16 x, UINT16 y,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;

    //ASSERT (Format != NULL);

    // Note this api do not supported ("%f", float)
    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
    /*
    if (StatusErrorCount % 61 == 0)
    {
        for (int j = 0; j < ScreenHeight - 25; j++)
        {
            for (int i = ScreenWidth / 2; i < ScreenWidth; i++)
            {
                pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
            }
        }       
    }
    */
    /*
    if (DisplayCount % 52 == 0)
    {
        for (int j = 2; j < 54 * 16; j++)
        {
            for (int i = 0; i < ScreenWidth / 4; i++)
            {
                pDeskBuffer[(j * ScreenWidth + i) * 4]     = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 1] = 0x84;
                pDeskBuffer[(j * ScreenWidth + i) * 4 + 2] = 0x00;
            }
        }       
    }
    */
    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw(WindowLayers.item[GRAPHICS_LAYER_DESK].pBuffer, x + i * 8, y, AsciiBuffer[i], Color);

}

VOID L2_STRING_Maker2 (UINT16 x, UINT16 y, WINDOW_LAYER_ITEM layer,
  IN  CONST CHAR8   *Format,
  IN  VA_LIST       VaList
  )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color;
    UINT32 i = 0;
    
    for (i = 0; i < 0x100; i++)
        AsciiBuffer[i] = 0;
        
    Color.Blue = 0xFF;
    Color.Red = 0xFF;
    Color.Green = 0xFF;
    Color.Reserved = layer.LayerID;

    //ASSERT (Format != NULL);

    AsciiVSPrint (AsciiBuffer, sizeof (AsciiBuffer), Format, VaList);
            
    for (i = 0; i < sizeof(AsciiBuffer) /sizeof(CHAR8); i++)
        L2_GRAPHICS_AsciiCharDraw2(layer, x + i * 8, y, AsciiBuffer[i], Color);

}

