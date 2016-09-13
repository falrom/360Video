// main.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "VRPicture.h"

/**
> ��������
- �����ļ�·����
- ��Ҫת����֡
*/
int main(int argc, char *argv[])
{
	cout << endl;
	cout << "**************************************************************" << endl;
	cout << "**                     ȫ��ͼƬ����任                     **" << endl;
	cout << "**                         By ���                          **" << endl;
	cout << "**************************************************************" << endl;
	cout << endl;

	VRPicture cVRP;
	try {
		cVRP.init(argv[1]);
	}
	catch (string s) {
		cout << "Error: " << s << endl;
		system("PAUSE");
		return 0;
	}

	int numOfFrames = atoi(argv[2]);
	double step = (double)atoi(argv[3]);

	cout << "YUV���·����	" << cVRP.getYUVPath() << endl;
	cout << "ͼƬ��ȣ�		" << cVRP.getRTGWidth() << endl;
	cout << "ͼƬ�߶ȣ�		" << cVRP.getRTGHeight() << endl;
	//cout << "�µ����߽��㣺   " << "(" << cVRP.getAim().lam << " , " << cVRP.getAim().phi << ")" << endl;

	//cout << endl << "��ʼ�任�����Ժ�..." << endl;
	//cVRP.outputYUV(".\\output.yuv");
	//cout << endl << "�ɹ���" << endl;

	//cout << endl << "��ʼ����ȫ���������ӽǣ����Ժ�..." << endl;
	//cVRP.perspectiveYUV(".\\show.yuv");
	//cout << endl << "�ɹ���" << endl;

	//cout << endl << "������..." << endl;
	//cVRP.outputOrigYUV();
	//cout << endl << "������ϡ�" << endl;

	for (int i = 0; i < numOfFrames; i++)
	{
		cVRP.importYUV(i);
		cout << endl << "Transforming frame " << i << "..." << endl;
		cVRP.perspectiveYUV(string(".\\origPlayer") + to_string(i) + string(".yuv"));
		SPHPosition tmpAim = cVRP.getAim();
		tmpAim.lam += step;
		cVRP.setAim(tmpAim);
	}

	// system("PAUSE");
	return 0;
}