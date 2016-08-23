#include "stdafx.h"
#include "VRPicture.h"

VRPicture::VRPicture()
{
	m_uiRTGWidth = 0;
	m_uiRTGHeight = 0;
	m_Aim = { 0.0, 0.0 };
	m_ucOrigImageY = NULL;
	m_ucOrigImageU = NULL;
	m_ucOrigImageV = NULL;
}


VRPicture::~VRPicture()
{
	delete m_ucOrigImageY;
	delete m_ucOrigImageU;
	delete m_ucOrigImageV;
}

/**
> 对象初始化函数。需要异常处理。
- cfgFilePath	:	配置文件的路径
*/
void VRPicture::init(const char * cfgFilePath)
{
	// 读取配置文件
	try { readCfg(cfgFilePath); }
	catch (string s) { throw; }
	// 载入YUV图像
	try { importYUV(); }
	catch (string s) { throw; }
}

/**
> 在配置文件中读取特定的变量。
- cfgFile	:	配置文件流
- key		:	文本中的变量名
- value		:	对应变量的值，用于保存
*/
bool VRPicture::readVariable(fstream & cfgFile, const string & key, string & value)
{
	char tmp[1000];
	cfgFile.clear();
	cfgFile.seekg(0);
	while (!cfgFile.eof()) // 循环读取每一行
	{
		cfgFile.getline(tmp, 1000);
		string line(tmp);
		size_t pos = line.find('='); // 找到每行的“=”位置，之前是key之后是value
		if (pos == string::npos)
			continue;
		string tmpKey = line.substr(0, pos); // 取“=”之前
		if (key == tmpKey)
		{
			value = line.substr(pos + 1); // 取“=”号之后
			return true;
		}
	}
	return false;
}

/**
> 读取配置文件。需要异常处理。
- cfgFilePath	:	配置文件的路径
*/
bool VRPicture::readCfg(const char * cfgFilePath)
{
	if (cfgFilePath == nullptr) // 如果运行程序时没有传入配置文件路径的参数却不做处理，会导致程序运行失败。
	{
		throw string("Require a config file path.");
	}

	fstream cfgFile;
	cfgFile.open(cfgFilePath); // 打开文件

	if (!cfgFile.is_open()) // 验证是否成功打开文件。
	{
		throw string("Can not open cfg file!");
	}

	// 定义临时变量并开始挨个读取参数：
	string tmpValue;
	unsigned int tmpui;

	// 读取宽度
	if (!readVariable(cfgFile, string(RTG_WIDTH), tmpValue))
	{
		cfgFile.close(); // 关闭文件
		throw (string("Can not find cfg: " RTG_WIDTH));
	}
	tmpui = stol(tmpValue);
	if ((tmpui % 2) == 0) // 宽度、高度值必须为偶数
	{
		m_uiRTGWidth = tmpui;
	}
	else
	{
		cfgFile.close(); // 关闭文件
		throw (string(RTG_WIDTH " must be a even number."));
	}

	// 读取高度
	if (!readVariable(cfgFile, string(RTG_HEIGHT), tmpValue))
	{
		cfgFile.close(); // 关闭文件
		throw (string("Can not find cfg: " RTG_HEIGHT));
	}
	tmpui = stol(tmpValue);
	if ((tmpui % 2) == 0) // 宽度、高度值必须为偶数
	{
		m_uiRTGHeight = tmpui;
	}
	else
	{
		cfgFile.close(); // 关闭文件
		throw (string(RTG_HEIGHT " must be a even number."));
	}

	// 读取YUV文件路径
	if (!readVariable(cfgFile, string(YUV_FILE_PATH), tmpValue))
	{
		cfgFile.close(); // 关闭文件
		throw (string("Can not find cfg: " YUV_FILE_PATH));
	}
	m_YUVFilePath = tmpValue;

	// 读取视线中央点的经度坐标lam
	if (!readVariable(cfgFile, string(AIM_LAM), tmpValue))
	{
		cfgFile.close(); // 关闭文件
		throw (string("Can not find cfg: " AIM_LAM));
	}
	m_Aim.lam = stod(tmpValue);

	// 读取视线中央点的纬度坐标phi
	if (!readVariable(cfgFile, string(AIM_PHI), tmpValue))
	{
		cfgFile.close(); // 关闭文件
		throw (string("Can not find cfg: " AIM_PHI));
	}
	m_Aim.phi = stod(tmpValue);

	cfgFile.close(); // 关闭文件

	return true;
}

/*
> 将坐标位置信息转化为存储YUV文件的字符数组的序号。
- position	:	坐标信息
- type		:	分量类型：Y、U、V（4:2:0）
*/
unsigned int VRPicture::position2NUM(RTGPosition position, YUVtype type)
{
	// 坐标越界检查
	if (position.x >= (m_uiRTGWidth >> 1))
	{
		position.x = (m_uiRTGWidth >> 1) - 1;
	}
	if (position.x < -(m_uiRTGWidth >> 1))
	{
		position.x = -(m_uiRTGWidth >> 1);
	}
	if (position.y >= (m_uiRTGHeight >> 1))
	{
		position.y = (m_uiRTGHeight >> 1) - 1;
	}
	if (position.y < -(m_uiRTGHeight >> 1))
	{
		position.y = -(m_uiRTGHeight >> 1);
	}

	switch (type)
	{
	case Y:
		return ((m_uiRTGHeight >> 1) - 1 - position.y) * m_uiRTGWidth + (m_uiRTGWidth >> 1) + position.x;
	case U:
	case V:
		return (((m_uiRTGHeight >> 1) - 1 - position.y) / 2) * (m_uiRTGWidth >> 1) + ((position.x + (m_uiRTGWidth >> 1)) / 2 + 1);
	default:
		return 0;
	}
}

/*
> 将球坐标转换为对应的直角坐标。
- position	:	球坐标输入
*/
RTGPosition VRPicture::SPH2RTG(SPHPosition position)
{
	RTGPosition result;
	result.x = (int)floor(RADIUS_DEG * position.lam);
	result.y = (int)floor(RADIUS_DEG * position.phi);
	return result;
	// return RTGPosition();
}

/*
> 将直角坐标转换为对应的球坐标。
- position	:	直角坐标输入
*/
SPHPosition VRPicture::RTG2SPH(RTGPosition position)
{
	SPHPosition result;
	result.lam = position.x / RADIUS_DEG;
	result.phi = position.y / RADIUS_DEG;
	return result;
	// return SPHPosition();
}

/*
> 将球坐标变换回有意义的取值范围内。用于坐标加减后可能超出范围的情况。
- position	:	限制前的坐标值
*/
SPHPosition VRPicture::SPHinRange(SPHPosition position)
{
	// > 定义返回值
	SPHPosition result;

	// 经度lam
	double lamT = (ceil(abs(position.lam) / 360) * 360 + position.lam);
	lamT = lamT - (int)lamT + ((int)lamT % 360);
	if (lamT < 180)
	{
		result.lam = lamT;
	}
	else
	{
		result.lam = lamT - 360;
	}
	// 纬度phi
	double phiT = (ceil(abs(position.phi) / 360) * 360 + position.phi);
	phiT = phiT - (int)phiT + ((int)phiT % 360);
	if (phiT < 90)
	{
		result.phi = phiT;
	}
	else if (phiT > 270)
	{
		result.phi = phiT - 360;
	}
	else
	{
		result.phi = 180 - phiT;
		// lam校正
		if (result.lam >= 0)
		{
			result.lam -= 180;
		}
		else
		{
			result.phi += 180;
		}
	}

	return result;
}

/**
> 将目光移动后，生成的新的以目光中心为中心的球坐标，对应到原始球坐标。得到的返回值就是这一点在原始球坐标的位置。
> 即输入的是某一点在新球坐标系下的坐标，输出的是该点在原始球坐标系下的坐标。
- newSPHPosition	:	某一点在新生成球坐标中的位置。
- sightAim			:	目光中心相在旧球坐标的位置，即新旧球坐标的中心偏差。
*/
SPHPosition VRPicture::transformSPH(SPHPosition newSPHPosition, SPHPosition sightAim)
{
	// > 定义返回值
	SPHPosition oldPosition;
	// > 运算需要用到的中间变量
	double theta1, theta2, r, lam2, phi2, x1, x2, y1, y2, z1, z2,
		lam1 = newSPHPosition.lam * PI / 180,
		phi1 = newSPHPosition.phi * PI / 180,
		detaphi = sightAim.phi * PI / 180;

	// 球面纵向旋转计算：先将球坐标转换为笛卡尔三维直角坐标系进行运算
	r = sqrt(1 - cos(phi1)*cos(phi1) * sin(lam1)*sin(lam1));
	x1 = cos(phi1) * cos(lam1);
	y1 = - cos(phi1) * sin(lam1);
	z1 = sin(phi1);
	
	// 特殊越界情况的处理
	double tmpCosTheta1 = x1 / r;
	if (tmpCosTheta1 > 1) { tmpCosTheta1 = 1; }
	if (tmpCosTheta1 < -1) { tmpCosTheta1 = -1; }

	if (z1 >= 0)
	{
		theta1 = acos(tmpCosTheta1);
	}
	else
	{
		theta1 = 2 * PI - acos(tmpCosTheta1);
	}
	theta2 = theta1 + detaphi;

	x2 = r * cos(theta2);
	z2 = r * sin(theta2);
	y2 = y1;

	phi2 = asin(z2);
	lam2 = asin((-y2) / cos(phi2));
	if (x2 < 0)
	{
		if (y2 >= 0)
		{
			lam2 = -lam2 - PI;
		}
		else
		{
			lam2 = PI - lam2;
		}
	}
	// 球面纵向旋转运算结束，赋值。
	oldPosition.lam = lam2 * 180 / PI;
	oldPosition.phi = phi2 * 180 / PI;

	// 球面横向旋转：经度叠加
	oldPosition.lam += sightAim.lam;
	// 限制坐标取值范围
	oldPosition = SPHinRange(oldPosition);
	
	/*/// 计算经度lam
	double detalam = 90 - abs(newSPHPosition.lam) - atan(cos(sightAim.phi * PI / 180) * cos(abs(newSPHPosition.lam) * PI / 180) / sin(abs(newSPHPosition.lam) * PI / 180)) / PI * 180;
	oldPosition.lam = newSPHPosition.lam + sightAim.lam;
	if (newSPHPosition.lam > 0)
	{
		oldPosition.lam += detalam;
	}
	else if (newSPHPosition.lam < 0)
	{
		oldPosition.lam -= detalam;
	}
	if (oldPosition.lam >= 180)
	{
		oldPosition.lam -= 360;
	}
	else if (oldPosition.lam < -180)
	{
		oldPosition.lam += 360;
	}

	/// 计算纬度phi
	double detaphi = asin(cos(abs(newSPHPosition.lam) * PI / 180) * sin((sightAim.phi + newSPHPosition.phi)* PI / 180)) / PI * 180 -
					 asin(cos(abs(newSPHPosition.lam) * PI / 180) * sin((               newSPHPosition.phi)* PI / 180)) / PI * 180;
	oldPosition.phi = newSPHPosition.phi + detaphi;
	if (oldPosition.phi >= 90)
	{
		oldPosition.phi = 180 - oldPosition.phi;
		if (oldPosition.lam >= 0)
		{
			oldPosition.lam -= 180;
		}
		else
		{
			oldPosition.lam += 180;
		}
	}
	else if (oldPosition.phi < -90)
	{
		oldPosition.phi = -180 - oldPosition.phi;
		if (oldPosition.lam >= 0)
		{
			oldPosition.lam -= 180;
		}
		else
		{
			oldPosition.lam += 180;
		}
	}*/

	return oldPosition;
	// return SPHPosition();
}

/**
> 将目光移动后，生成的新的以目光中心为中心的直角坐标，对应到原始直角坐标。得到的返回值就是这一点在原始直角坐标的位置。
> 即输入的是某一点在新直角坐标系下的坐标，输出的是该点在原始直角坐标系下的坐标。

> newRTG ===> newSPH ===> oldSPH ===> oldRTG
>         |           |           |
>      RTG2SPH   transformSPH  SPH2RTG

- newRTGPosition	:	某一点在新生成直角坐标中的位置。
- sightAim			:	目光中心相在旧球坐标的位置，即新旧球坐标的中心偏差。
*/
RTGPosition VRPicture::transformRTG(RTGPosition newRTGPosition, SPHPosition sightAim)
{
	return SPH2RTG(transformSPH(RTG2SPH(newRTGPosition), sightAim));
}

/**
> 导入原始YUV图片文件。需要异常处理。
- numOfFrame	:	希望导入YUV文件的第几帧。从0开始算起。默认为0。
*/
bool VRPicture::importYUV(int numOfFrame)
{
	// Open file.
	ifstream origYUV(m_YUVFilePath, ios::binary);
	if (!origYUV.is_open())
	{
		throw (string("Can't open YUV file: ") + m_YUVFilePath);
	}
	// Read.
	int tmpGetPosi = numOfFrame * ((SIZE_OF_RTG >> 1) + SIZE_OF_RTG);
	origYUV.seekg(tmpGetPosi, ios::beg);
	m_ucOrigImageY = new unsigned char[SIZE_OF_RTG];
	m_ucOrigImageU = new unsigned char[SIZE_OF_RTG >> 2];
	m_ucOrigImageV = new unsigned char[SIZE_OF_RTG >> 2];
	origYUV.read((char *)m_ucOrigImageY, SIZE_OF_RTG);
	origYUV.read((char *)m_ucOrigImageU, SIZE_OF_RTG >> 2);
	origYUV.read((char *)m_ucOrigImageV, SIZE_OF_RTG >> 2);
	// Close file.
	origYUV.close();

	return true;
}

/*
> 获取原始YUV文件特定位置Y分量。
- position	:	寻找位置。
*/
unsigned char VRPicture::getOrigY(RTGPosition position)
{
	return m_ucOrigImageY[position2NUM(position, Y)];
}

/*
> 获取原始YUV文件特定位置U分量。
- position	:	寻找位置。
*/
unsigned char VRPicture::getOrigU(RTGPosition position)
{
	return m_ucOrigImageU[position2NUM(position, U)];
}

/*
> 获取原始YUV文件特定位置V分量。
- position	:	寻找位置。
*/
unsigned char VRPicture::getOrigV(RTGPosition position)
{
	return m_ucOrigImageV[position2NUM(position, V)];
}

/*
> 按照视线位置在特定目录生成新的矩形YUV文件。依旧采用全景图片的球形投影方式： Equirectangular
- path		:	生成文件的路径和名称
- sightAim	:	视线中央的球坐标
*/
void VRPicture::outputYUV(string path, SPHPosition sightAim)
{
	ofstream output;
	output.open(path, ios::binary);

	RTGPosition tmpPosition;
	char *tmpPC = new char;

	// Y
	for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y--)
	{
		for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x++)
		{
			*tmpPC = (char)getOrigY(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}
	// U
	for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y = tmpPosition.y - 2)
	{
		for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x = tmpPosition.x + 2)
		{
			*tmpPC = (char)getOrigU(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}
	// V
	for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y = tmpPosition.y - 2)
	{
		for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x = tmpPosition.x + 2)
		{
			*tmpPC = (char)getOrigV(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}

	delete tmpPC;
	output.close();
}

/**
> 按照视线位置在特定目录生成新的矩形YUV文件。依旧采用全景图片的球形投影方式： Equirectangular
> 视线中央的球坐标使用成员m_Aim。
- path		:	生成文件的路径
*/
void VRPicture::outputYUV(string path)
{
	outputYUV(path, m_Aim);
}

/**
> 投影函数：透视投影（perspective projection），在球心观察球面时画面的平面投影。
> 输入投影平面某点坐标值，得到在球面上对应点的坐标值
- position	:	在投影平面上的坐标值
*/
SPHPosition VRPicture::perspective(RTGPosition position)
{
	SPHPosition result;

	double tmplam = atan(position.x / RADIUS_RAD);
	result.lam = tmplam * 180 / PI;
	result.phi = atan(position.y / RADIUS_RAD * cos(tmplam)) * 180 / PI;
	// result.phi = atan(position.y / RADIUS_RAD) * 180 / PI;

	return result;
	// return SPHPosition();
}

/**
> 文件生成：透视投影（perspective projection），在球心观察球面时画面的平面投影。
> 生成一个特定位置为中心的矩形的YUV格式图片，投影方式为透视投影。即全景视频的播放窗口。
- path		:	输出文件的路径和名称
- width		:	生成图片的宽度
- heigh		:	生成图片的高度
- sightAim	:	中心点的球坐标
*/
void VRPicture::perspectiveYUV(string path, double viewAngleWidth, double viewAngleHeight, SPHPosition sightAim)
{
	viewAngleWidth  = viewAngleWidth  * PI / 180; // DEG=>RAD
	viewAngleHeight = viewAngleHeight * PI / 180;
	int width = (int)abs(RADIUS_RAD * tan(viewAngleWidth / 2.0));
	int height = (int)abs(RADIUS_RAD * tan(viewAngleHeight / 2.0));
	width *= 2;
	height *= 2;

	// 宽高必须是4的倍数才能用来编码
	int tmpWr = width  % 4;
	int tmpHr = height % 4;
	if (tmpWr != 0)
	{
		width  = width  + 4 - tmpWr;
	}
	if (tmpHr != 0)
	{
		height = height + 4 - tmpHr;
	}

	ofstream output;
	output.open(path, ios::binary);

	RTGPosition tmpPosition;
	char *tmpPC = new char;

	// Y
	for (tmpPosition.y = (height >> 1) - 1; tmpPosition.y >= -(height >> 1); tmpPosition.y--)
	{
		for (tmpPosition.x = -(width >> 1); tmpPosition.x < (width >> 1); tmpPosition.x++)
		{
			*tmpPC = (char)getOrigY(SPH2RTG(transformSPH(perspective(tmpPosition), sightAim)));
			// *tmpPC = (char)getOrigY(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}
	// U
	for (tmpPosition.y = (height >> 1) - 1; tmpPosition.y >= -(height >> 1); tmpPosition.y = tmpPosition.y - 2)
	{
		for (tmpPosition.x = -(width >> 1); tmpPosition.x < (width >> 1); tmpPosition.x = tmpPosition.x + 2)
		{
			*tmpPC = (char)getOrigU(SPH2RTG(transformSPH(perspective(tmpPosition), sightAim)));
			// *tmpPC = (char)getOrigU(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}
	// V
	for (tmpPosition.y = (height >> 1) - 1; tmpPosition.y >= -(height >> 1); tmpPosition.y = tmpPosition.y - 2)
	{
		for (tmpPosition.x = -(width >> 1); tmpPosition.x < (width >> 1); tmpPosition.x = tmpPosition.x + 2)
		{
			*tmpPC = (char)getOrigV(SPH2RTG(transformSPH(perspective(tmpPosition), sightAim)));
			// *tmpPC = (char)getOrigV(transformRTG(tmpPosition, sightAim));
			output.write(tmpPC, 1);
		}
	}

	delete tmpPC;
	output.close();
}

/**
> 文件生成：透视投影（perspective projection），在球心观察球面时画面的平面投影。
> 生成一个特定位置为中心的矩形的YUV格式图片，投影方式为透视投影。即全景视频的播放窗口。
> 重载。使用对象属性得到默认参数。
- path		:	输出文件的路径和名称
*/
void VRPicture::perspectiveYUV(string path)
{
	//double viewAngleWidth  = 90 * PI / 180; // DEG=>RAD
	//double viewAngleHeight = 90 * PI / 180;
	//int _width  = (int)abs(RADIUS_RAD * tan(viewAngleWidth  / 2.0));
	//int _height = (int)abs(RADIUS_RAD * tan(viewAngleHeight / 2.0));
	//_width  *= 2;
	//_height *= 2;

	perspectiveYUV(path, 90, 90, m_Aim);
}

/**
> 测试函数：依照读取数据重新生成原YUV文件，用于测试YUV文件的读取情况。
*/
void VRPicture::outputOrigYUV()
{
	ofstream output;
	output.open(".\\outputOrig.yuv", ios::binary);

	output.write((char *)m_ucOrigImageY, SIZE_OF_RTG);
	output.write((char *)m_ucOrigImageU, SIZE_OF_RTG >> 2);
	output.write((char *)m_ucOrigImageV, SIZE_OF_RTG >> 2);



	//RTGPosition tmpPosition;
	//char *tmpPC = new char;

	//// Y
	//for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y--)
	//{
	//	for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x++)
	//	{
	//		*tmpPC = (char)getOrigY(transformRTG(tmpPosition, sightAim));
	//		output.write(tmpPC, 1);
	//	}
	//}
	//// U
	//for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y = tmpPosition.y - 2)
	//{
	//	for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x = tmpPosition.x + 2)
	//	{
	//		*tmpPC = (char)getOrigU(transformRTG(tmpPosition, sightAim));
	//		output.write(tmpPC, 1);
	//	}
	//}
	//// V
	//for (tmpPosition.y = (m_uiRTGHeight >> 1) - 1; tmpPosition.y >= -(m_uiRTGHeight >> 1); tmpPosition.y = tmpPosition.y - 2)
	//{
	//	for (tmpPosition.x = -(m_uiRTGWidth >> 1); tmpPosition.x < (m_uiRTGWidth >> 1); tmpPosition.x = tmpPosition.x + 2)
	//	{
	//		*tmpPC = (char)getOrigV(transformRTG(tmpPosition, sightAim));
	//		output.write(tmpPC, 1);
	//	}
	//}

	//delete tmpPC;
	output.close();
}
