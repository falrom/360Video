#pragma once

#include <iostream>
#include <fstream>
#include <string> 

// > Configs which need to be read from cfgFile.��Ҫ�������ļ��ж�ȡ�ı����ļ���
#define YUV_FILE_PATH	"YUVFilePath"						// > ԭʼYUVͼƬ�ļ���·��
#define RTG_WIDTH		"RTGWidth"							// > ԭʼYUVͼƬ�ļ��Ŀ��
#define RTG_HEIGHT		"RTGHeight"							// > ԭʼYUVͼƬ�ļ��ĸ߶�
#define AIM_LAM			"SightAimLambda"					// > �����������������lam����
#define AIM_PHI			"SightAimPhi"						// > �����������������phiγ��

// > �����궨��
#define PI				3.14159265							// > ������
#define SIZE_OF_RTG		(m_uiRTGWidth * m_uiRTGHeight)		// > ����ͼ��Ĵ�С�����˿�
#define RADIUS_DEG		(m_uiRTGWidth / 360.0)				// > ��������任�õ�����뾶���Ƕȵ�λ���ȣ�
#define RADIUS_RAD		(m_uiRTGWidth / 2.0 / PI )			// > ��������任�õ�����뾶���Ƕȵ�λ�����ȣ�

using namespace std;

// > rectangle coordinate ���λ�������
typedef struct tagRTGPosition
{
	int x;			// > ������
	int y;			// > ������
}RTGPosition;

// > sphere coordinate ���λ������꣨ʹ�ã��Ƕȣ�
typedef struct tagSPHPosition
{
	double lam;		// > Lambada (��) ���ȣ��Ƕȣ�
	double phi;		// > Phi     (��) γ�ȣ��Ƕȣ�
}SPHPosition;

// > ����YUV������ʶ
enum YUVtype { Y, U, V };

/******************************************************************************************
> 360VRȫ��ͼ���ඨ�塣
> ���ڶ�ȫ��ͼ�����ض��Ĵ���ͱ任��
******************************************************************************************/
class VRPicture
{
private:
	
	/****************************** ˽�������� ******************************/
	SPHPosition m_Aim;							// > central of sight.Ŀ�������λ��
	string m_YUVFilePath;						// > YUV file path.(4:2:0)
	int m_uiRTGWidth;							// > width  of rectangle picture.������ͼ��
	int m_uiRTGHeight;							// > height of rectangle picture.������ͼ��
	const unsigned char* m_ucOrigImageY;		// > ԭʼYUVͼ��Y����
	const unsigned char* m_ucOrigImageU;		// > ԭʼYUVͼ��U����
	const unsigned char* m_ucOrigImageV;		// > ԭʼYUVͼ��V����

	/****************************** ˽���ຯ�� ******************************/
	// > �������ļ��ж�ȡ�ض�������ֵ
	bool readVariable(fstream & cfgFile, const string & key, string & value);

	// > ��λ����Ϣת��Ϊ�洢ͼ��������ж�Ӧ�����
	unsigned int position2NUM(RTGPosition position, YUVtype type);

	// > ����ת�������α����
	RTGPosition SPH2RTG(SPHPosition position);

	// > ����ת�������α�����
	SPHPosition RTG2SPH(RTGPosition position);

	// > ���������������������ȡֵ��Χ��
	SPHPosition SPHinRange(SPHPosition position);

	// > ��ȡ�µ���������ĳ����ԭ������Ķ�Ӧλ�á�
	SPHPosition transformSPH(SPHPosition newSPHPosition, SPHPosition sightAim);

	// > ��ȡ�µ�ֱ��������ĳ����ԭֱ������Ķ�Ӧλ�á�
	RTGPosition transformRTG(RTGPosition newRTGPosition, SPHPosition sightAim);

public:

	/****************************** �����ຯ�� ******************************/
	VRPicture();
	~VRPicture();

	// > ��ʼ�����������ڶ���ĳ�ʼ���趨��ʹ�������ʵ������
	void init(const char * cfgFilePath);

	// > �趨����������������
	void setAim(SPHPosition Aim) { m_Aim = Aim; }

	// > ��ȡ����������������
	SPHPosition getAim() { return m_Aim; }

	// > ��ȡԭʼYUVͼ��Ŀ��
	unsigned int getRTGWidth() { return m_uiRTGWidth; }

	// > ��ȡԭʼYUVͼ��ĸ߶�
	unsigned int getRTGHeight() { return m_uiRTGHeight; }
	
	// > ��ȡ�����ļ�	
	bool readCfg(const char * cfgFilePath);

	// > ��ȡԭʼYUV�ļ��Ĵ洢·��
	string getYUVPath() { return m_YUVFilePath; }

	// > ����ԭʼYUV�ļ�
	bool importYUV(int numOfFrame = 0);

	// > ��ȡYUV�ļ��ض�λ��Y����
	unsigned char getOrigY(RTGPosition position);

	// > ��ȡYUV�ļ��ض�λ��U����
	unsigned char getOrigU(RTGPosition position);

	// > ��ȡYUV�ļ��ض�λ��V����
	unsigned char getOrigV(RTGPosition position);
	
	// > ����ȫ��ͼͶӰ��ʽ��Equirectangular���������ɸı������ĵ���µ�YUV�ļ�ȫ��ͼ
	void outputYUV(string path, SPHPosition sightAim);

	// > ����ȫ��ͼͶӰ��ʽ��Equirectangular���������ɸı������ĵ���µ�YUV�ļ�ȫ��ͼ��Ĭ��ʹ�ó�Աm_Aim��
	void outputYUV(string path);

	// ==================== ����ͶӰ��ʽ�Լ��ļ����� ====================

	// > ͶӰ������͸��ͶӰ ����ȫ����Ƶ����
	SPHPosition perspective(RTGPosition position);

	// > �ļ����ɣ�͸��ͶӰ ����ȫ����Ƶ����
	void perspectiveYUV(string path, double viewAngleWidth, double viewAngleHeight, SPHPosition sightAim);

	// > �ļ����ɣ�͸��ͶӰ ����ȫ����Ƶ���� <Ĭ�ϲ���>
	void perspectiveYUV(string path);
	
	// ==================== ���ԡ������ú��� ====================

	// > ���Ժ�������������ԭYUV�ļ�
	void outputOrigYUV();

};

