#pragma once

#include <iostream>
#include <fstream>
#include <string> 

// > Configs which need to be read from cfgFile.需要从配置文件中读取的变量的键。
#define YUV_FILE_PATH	"YUVFilePath"						// > 原始YUV图片文件的路径
#define RTG_WIDTH		"RTGWidth"							// > 原始YUV图片文件的宽度
#define RTG_HEIGHT		"RTGHeight"							// > 原始YUV图片文件的高度
#define AIM_LAM			"SightAimLambda"					// > 视线中央点的球坐标的lam经度
#define AIM_PHI			"SightAimPhi"						// > 视线中央点的球坐标的phi纬度

// > 变量宏定义
#define SIZE_OF_RTG		(m_uiRTGWidth * m_uiRTGHeight)		// > 矩形图像的大小。长乘宽。
#define RADIUS			(m_uiRTGWidth / 360.0)				// > 定义坐标变换用到的球半径
#define PI				3.14159265							// > 定义派

using namespace std;

// > rectangle coordinate 矩形画面坐标
typedef struct tagRTGPosition
{
	int x;			// > 横坐标
	int y;			// > 纵坐标
}RTGPosition;

// > sphere coordinate 球形画面坐标（使用：角度）
typedef struct tagSPHPosition
{
	double lam;		// > Lambada (λ) 经度（角度）
	double phi;		// > Phi     (φ) 纬度（角度）
}SPHPosition;

// > 定义YUV分量标识
enum YUVtype { Y, U, V };

/******************************************************************************************
> 360VR全景图像类定义。
> 用于对全景图像做特定的处理和变换。
******************************************************************************************/
class VRPicture
{
private:
	
	/****************************** 私有类属性 ******************************/
	SPHPosition m_Aim;							// > central of sight.目光的中央位置
	string m_YUVFilePath;						// > YUV file path.(4:2:0)
	int m_uiRTGWidth;							// > width  of rectangle picture.矩形视图宽
	int m_uiRTGHeight;							// > height of rectangle picture.矩形视图高
	const unsigned char* m_ucOrigImageY;		// > 原始YUV图像Y分量
	const unsigned char* m_ucOrigImageU;		// > 原始YUV图像U分量
	const unsigned char* m_ucOrigImageV;		// > 原始YUV图像V分量

	/****************************** 私有类函数 ******************************/
	// > 在配置文件中读取特定变量的值
	bool readVariable(fstream & cfgFile, const string & key, string & value);

	// > 将位置信息转换为存储图像的数组中对应的序号
	unsigned int position2NUM(RTGPosition position, YUVtype type);

	// > 坐标转换：球形变矩形
	RTGPosition SPH2RTG(SPHPosition position);

	// > 坐标转换：矩形变球形
	SPHPosition RTG2SPH(RTGPosition position);

	// > 将球坐标限制在有意义的取值范围内
	SPHPosition SPHinRange(SPHPosition position);

	// > 获取新的球坐标中某点在原球坐标的对应位置。
	SPHPosition transformSPH(SPHPosition newSPHPosition, SPHPosition sightAim);

	// > 获取新的直角坐标中某点在原直角坐标的对应位置。
	RTGPosition transformRTG(RTGPosition newRTGPosition, SPHPosition sightAim);

public:

	/****************************** 公有类函数 ******************************/
	VRPicture();
	~VRPicture();

	// > 初始化函数，用于对象的初始化设定，使对象具有实际意义
	void init(const char * cfgFilePath);

	// > 设定视线中央点的球坐标
	void setAim(SPHPosition Aim) { m_Aim = Aim; }

	// > 获取视线中央点的球坐标
	SPHPosition getAim() { return m_Aim; }

	// > 获取原始YUV图像的宽度
	unsigned int getRTGWidth() { return m_uiRTGWidth; }

	// > 获取原始YUV图像的高度
	unsigned int getRTGHeight() { return m_uiRTGHeight; }
	
	// > 读取配置文件	
	bool readCfg(const char * cfgFilePath);

	// > 获取原始YUV文件的存储路径
	string getYUVPath() { return m_YUVFilePath; }

	// > 载入原始YUV文件
	bool importYUV();

	// > 获取YUV文件特定位置Y分量
	unsigned char getOrigY(RTGPosition position);

	// > 获取YUV文件特定位置U分量
	unsigned char getOrigU(RTGPosition position);

	// > 获取YUV文件特定位置V分量
	unsigned char getOrigV(RTGPosition position);
	
	// > 按照特定要求生成新的YUV文件
	void outputYUV(string path, SPHPosition sightAim);

	// > 按照特定要求生成新的YUV文件（默认使用成员m_Aim）
	void outputYUV(string path);
	
};

