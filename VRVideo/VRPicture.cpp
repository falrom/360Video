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
> �����ʼ����������Ҫ�쳣����
- cfgFilePath	:	�����ļ���·��
*/
void VRPicture::init(const char * cfgFilePath)
{
	// ��ȡ�����ļ�
	try { readCfg(cfgFilePath); }
	catch (string s) { throw; }
	// ����YUVͼ��
	try { importYUV(); }
	catch (string s) { throw; }
}

/**
> �������ļ��ж�ȡ�ض��ı�����
- cfgFile	:	�����ļ���
- key		:	�ı��еı�����
- value		:	��Ӧ������ֵ�����ڱ���
*/
bool VRPicture::readVariable(fstream & cfgFile, const string & key, string & value)
{
	char tmp[1000];
	cfgFile.clear();
	cfgFile.seekg(0);
	while (!cfgFile.eof()) // ѭ����ȡÿһ��
	{
		cfgFile.getline(tmp, 1000);
		string line(tmp);
		size_t pos = line.find('='); // �ҵ�ÿ�еġ�=��λ�ã�֮ǰ��key֮����value
		if (pos == string::npos)
			continue;
		string tmpKey = line.substr(0, pos); // ȡ��=��֮ǰ
		if (key == tmpKey)
		{
			value = line.substr(pos + 1); // ȡ��=����֮��
			return true;
		}
	}
	return false;
}

/**
> ��ȡ�����ļ�����Ҫ�쳣����
- cfgFilePath	:	�����ļ���·��
*/
bool VRPicture::readCfg(const char * cfgFilePath)
{
	if (cfgFilePath == nullptr) // ������г���ʱû�д��������ļ�·���Ĳ���ȴ���������ᵼ�³�������ʧ�ܡ�
	{
		throw string("Require a config file path.");
	}

	fstream cfgFile;
	cfgFile.open(cfgFilePath); // ���ļ�

	if (!cfgFile.is_open()) // ��֤�Ƿ�ɹ����ļ���
	{
		throw string("Can not open cfg file!");
	}

	// ������ʱ��������ʼ������ȡ������
	string tmpValue;
	unsigned int tmpui;

	// ��ȡ���
	if (!readVariable(cfgFile, string(RTG_WIDTH), tmpValue))
	{
		cfgFile.close(); // �ر��ļ�
		throw (string("Can not find cfg: " RTG_WIDTH));
	}
	tmpui = stol(tmpValue);
	if ((tmpui % 2) == 0) // ��ȡ��߶�ֵ����Ϊż��
	{
		m_uiRTGWidth = tmpui;
	}
	else
	{
		cfgFile.close(); // �ر��ļ�
		throw (string(RTG_WIDTH " must be a even number."));
	}

	// ��ȡ�߶�
	if (!readVariable(cfgFile, string(RTG_HEIGHT), tmpValue))
	{
		cfgFile.close(); // �ر��ļ�
		throw (string("Can not find cfg: " RTG_HEIGHT));
	}
	tmpui = stol(tmpValue);
	if ((tmpui % 2) == 0) // ��ȡ��߶�ֵ����Ϊż��
	{
		m_uiRTGHeight = tmpui;
	}
	else
	{
		cfgFile.close(); // �ر��ļ�
		throw (string(RTG_HEIGHT " must be a even number."));
	}

	// ��ȡYUV�ļ�·��
	if (!readVariable(cfgFile, string(YUV_FILE_PATH), tmpValue))
	{
		cfgFile.close(); // �ر��ļ�
		throw (string("Can not find cfg: " YUV_FILE_PATH));
	}
	m_YUVFilePath = tmpValue;

	// ��ȡ���������ľ�������lam
	if (!readVariable(cfgFile, string(AIM_LAM), tmpValue))
	{
		cfgFile.close(); // �ر��ļ�
		throw (string("Can not find cfg: " AIM_LAM));
	}
	m_Aim.lam = stod(tmpValue);

	// ��ȡ����������γ������phi
	if (!readVariable(cfgFile, string(AIM_PHI), tmpValue))
	{
		cfgFile.close(); // �ر��ļ�
		throw (string("Can not find cfg: " AIM_PHI));
	}
	m_Aim.phi = stod(tmpValue);

	cfgFile.close(); // �ر��ļ�

	return true;
}

/*
> ������λ����Ϣת��Ϊ�洢YUV�ļ����ַ��������š�
- position	:	������Ϣ
- type		:	�������ͣ�Y��U��V��4:2:0��
*/
unsigned int VRPicture::position2NUM(RTGPosition position, YUVtype type)
{
	// ����Խ����
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
> ��������ת��Ϊ��Ӧ��ֱ�����ꡣ
- position	:	����������
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
> ��ֱ������ת��Ϊ��Ӧ�������ꡣ
- position	:	ֱ����������
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
> ��������任���������ȡֵ��Χ�ڡ���������Ӽ�����ܳ�����Χ�������
- position	:	����ǰ������ֵ
*/
SPHPosition VRPicture::SPHinRange(SPHPosition position)
{
	// > ���巵��ֵ
	SPHPosition result;

	// ����lam
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
	// γ��phi
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
		// lamУ��
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
> ��Ŀ���ƶ������ɵ��µ���Ŀ������Ϊ���ĵ������꣬��Ӧ��ԭʼ�����ꡣ�õ��ķ���ֵ������һ����ԭʼ�������λ�á�
> ���������ĳһ������������ϵ�µ����꣬������Ǹõ���ԭʼ������ϵ�µ����ꡣ
- newSPHPosition	:	ĳһ�����������������е�λ�á�
- sightAim			:	Ŀ���������ھ��������λ�ã����¾������������ƫ�
*/
SPHPosition VRPicture::transformSPH(SPHPosition newSPHPosition, SPHPosition sightAim)
{
	// > ���巵��ֵ
	SPHPosition oldPosition;
	// > ������Ҫ�õ����м����
	double theta1, theta2, r, lam2, phi2, x1, x2, y1, y2, z1, z2,
		lam1 = newSPHPosition.lam * PI / 180,
		phi1 = newSPHPosition.phi * PI / 180,
		detaphi = sightAim.phi * PI / 180;

	// ����������ת���㣺�Ƚ�������ת��Ϊ�ѿ�����άֱ������ϵ��������
	r = sqrt(1 - cos(phi1)*cos(phi1) * sin(lam1)*sin(lam1));
	x1 = cos(phi1) * cos(lam1);
	y1 = - cos(phi1) * sin(lam1);
	z1 = sin(phi1);
	
	// ����Խ������Ĵ���
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
	// ����������ת�����������ֵ��
	oldPosition.lam = lam2 * 180 / PI;
	oldPosition.phi = phi2 * 180 / PI;

	// ���������ת�����ȵ���
	oldPosition.lam += sightAim.lam;
	// ��������ȡֵ��Χ
	oldPosition = SPHinRange(oldPosition);
	
	/*/// ���㾭��lam
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

	/// ����γ��phi
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
> ��Ŀ���ƶ������ɵ��µ���Ŀ������Ϊ���ĵ�ֱ�����꣬��Ӧ��ԭʼֱ�����ꡣ�õ��ķ���ֵ������һ����ԭʼֱ�������λ�á�
> ���������ĳһ������ֱ������ϵ�µ����꣬������Ǹõ���ԭʼֱ������ϵ�µ����ꡣ

> newRTG ===> newSPH ===> oldSPH ===> oldRTG
>         |           |           |
>      RTG2SPH   transformSPH  SPH2RTG

- newRTGPosition	:	ĳһ����������ֱ�������е�λ�á�
- sightAim			:	Ŀ���������ھ��������λ�ã����¾������������ƫ�
*/
RTGPosition VRPicture::transformRTG(RTGPosition newRTGPosition, SPHPosition sightAim)
{
	return SPH2RTG(transformSPH(RTG2SPH(newRTGPosition), sightAim));
}

/**
> ����ԭʼYUVͼƬ�ļ�����Ҫ�쳣����
- numOfFrame	:	ϣ������YUV�ļ��ĵڼ�֡����0��ʼ����Ĭ��Ϊ0��
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
> ��ȡԭʼYUV�ļ��ض�λ��Y������
- position	:	Ѱ��λ�á�
*/
unsigned char VRPicture::getOrigY(RTGPosition position)
{
	return m_ucOrigImageY[position2NUM(position, Y)];
}

/*
> ��ȡԭʼYUV�ļ��ض�λ��U������
- position	:	Ѱ��λ�á�
*/
unsigned char VRPicture::getOrigU(RTGPosition position)
{
	return m_ucOrigImageU[position2NUM(position, U)];
}

/*
> ��ȡԭʼYUV�ļ��ض�λ��V������
- position	:	Ѱ��λ�á�
*/
unsigned char VRPicture::getOrigV(RTGPosition position)
{
	return m_ucOrigImageV[position2NUM(position, V)];
}

/*
> ��������λ�����ض�Ŀ¼�����µľ���YUV�ļ������ɲ���ȫ��ͼƬ������ͶӰ��ʽ�� Equirectangular
- path		:	�����ļ���·��������
- sightAim	:	���������������
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
> ��������λ�����ض�Ŀ¼�����µľ���YUV�ļ������ɲ���ȫ��ͼƬ������ͶӰ��ʽ�� Equirectangular
> ���������������ʹ�ó�Աm_Aim��
- path		:	�����ļ���·��
*/
void VRPicture::outputYUV(string path)
{
	outputYUV(path, m_Aim);
}

/**
> ͶӰ������͸��ͶӰ��perspective projection���������Ĺ۲�����ʱ�����ƽ��ͶӰ��
> ����ͶӰƽ��ĳ������ֵ���õ��������϶�Ӧ�������ֵ
- position	:	��ͶӰƽ���ϵ�����ֵ
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
> �ļ����ɣ�͸��ͶӰ��perspective projection���������Ĺ۲�����ʱ�����ƽ��ͶӰ��
> ����һ���ض�λ��Ϊ���ĵľ��ε�YUV��ʽͼƬ��ͶӰ��ʽΪ͸��ͶӰ����ȫ����Ƶ�Ĳ��Ŵ��ڡ�
- path		:	����ļ���·��������
- width		:	����ͼƬ�Ŀ��
- heigh		:	����ͼƬ�ĸ߶�
- sightAim	:	���ĵ��������
*/
void VRPicture::perspectiveYUV(string path, double viewAngleWidth, double viewAngleHeight, SPHPosition sightAim)
{
	viewAngleWidth  = viewAngleWidth  * PI / 180; // DEG=>RAD
	viewAngleHeight = viewAngleHeight * PI / 180;
	int width = (int)abs(RADIUS_RAD * tan(viewAngleWidth / 2.0));
	int height = (int)abs(RADIUS_RAD * tan(viewAngleHeight / 2.0));
	width *= 2;
	height *= 2;

	// ��߱�����4�ı���������������
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
> �ļ����ɣ�͸��ͶӰ��perspective projection���������Ĺ۲�����ʱ�����ƽ��ͶӰ��
> ����һ���ض�λ��Ϊ���ĵľ��ε�YUV��ʽͼƬ��ͶӰ��ʽΪ͸��ͶӰ����ȫ����Ƶ�Ĳ��Ŵ��ڡ�
> ���ء�ʹ�ö������Եõ�Ĭ�ϲ�����
- path		:	����ļ���·��������
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
> ���Ժ��������ն�ȡ������������ԭYUV�ļ������ڲ���YUV�ļ��Ķ�ȡ�����
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
