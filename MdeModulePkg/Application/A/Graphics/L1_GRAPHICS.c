
/*************************************************
    .    
    File name:      	*.*
    Author：	        	任启红
    ID：					00001
    Date:          		202107
    Description:    	图形处理的一些基础算法，不依赖其他模块
    Others:         	无
    
    History:        	无
	    1.  Date:
		    Author: 
		    ID:
		    Modification:
		    
	    2.  Date:
		    Author: 
		    ID:
		    Modification:
*************************************************/





#include <L1_GRAPHICS.h>

WINDOW_LAYERS WindowLayers;



double L1_GRAPHICS_Bilinear(double a, double blue, int uv, int u1v, int uv1, int u1v1)
{
    return (double) (uv*(1-a)*(1-blue)+u1v*a*(1-blue)+uv1*blue*(1-a)+u1v1*a*blue);
}

void L1_GRAPHICS_ZoomImage(UINT8* pDest, int DestWidth, int DestHeight, UINT8* pSource, int SourceWidth, int SourceHeight )
{
    UINT8    *pDestTemp;
    UINT8    *pSourceTemp;
    UINT16    i, j;
    pDestTemp    = pDest;
    pSourceTemp  = pSource;


    for ( i = 0; i < DestHeight; i++ )
    {
        for ( j = 0; j < DestWidth; j++ )
        {
            int    HeightRatio, WidthRatio;
            double    u, v, red, green, blue;
            v    = (i * SourceHeight) / (double) (DestHeight);
            u    = (j * SourceWidth) / (double) (DestWidth);
            HeightRatio    = (int) (v);
            WidthRatio    = (int) (u);

            blue = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3] );
                      
            green = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3 + 1],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3 + 1],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3 + 1],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3 + 1] );
                      
            red = L1_GRAPHICS_Bilinear( u - WidthRatio, v - HeightRatio,
                      pSourceTemp[HeightRatio * SourceWidth * 3 + WidthRatio * 3 + 2],
                      pSourceTemp[HeightRatio * SourceWidth * 3 + (WidthRatio + 1) * 3 + 2],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + WidthRatio * 3 + 2],
                      pSourceTemp[(HeightRatio + 1) * SourceWidth * 3 + (WidthRatio + 1) * 3 + 2] );

            pDestTemp[i * DestWidth * 3 + j * 3 + 2]    = red;
            pDestTemp[i * DestWidth * 3 + j * 3 + 1]    = green;
            pDestTemp[i * DestWidth * 3 + j * 3 + 0]    = blue;
        }
    }
}


