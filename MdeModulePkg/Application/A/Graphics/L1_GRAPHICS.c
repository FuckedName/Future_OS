
/*************************************************
    .    
    File name:      	*.*
    Author��	        	������
    ID��					00001
    Date:          		202107
    Description:    	ͼ�δ����һЩ�����㷨������������ģ��
    Others:         	��
    
    History:        	��
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








/****************************************************************************
*
*  ����:   ˫���Բ�ֵBilinear Interpolation�㷨
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
unsigned char L1_GRAPHICS_Bilinear(double a, double blue, int uv, int u1v, int uv1, int u1v1)
{
    return (unsigned char) (uv*(1-a)*(1-blue)+u1v*a*(1-blue)+uv1*blue*(1-a)+u1v1*a*blue);
}




/****************************************************************************
*
*  ����: ͼƬ��С�㷨����pSourceͼƬ��ȡ��߶���С��pDest�Ŀ�Ⱥ͸߶�
*
*  ����1�� xxxxx
*  ����2�� xxxxx
*  ����n�� xxxxx
*
*  ����ֵ�� �ɹ���XXXX��ʧ�ܣ�XXXXX
*
*****************************************************************************/
void L1_GRAPHICS_ZoomImage(UINT8* pDest, unsigned int DestWidth, unsigned int DestHeight, UINT8* pSource, unsigned int SourceWidth, unsigned int SourceHeight )
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
            double    u, v;
			unsigned char red, green, blue;
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


