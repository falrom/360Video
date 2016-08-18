// main.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "VRPicture.h"

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

	cout << "YUVͼƬ���·����" << cVRP.getYUVPath() << endl;
	cout << "ͼƬ��ȣ�       " << cVRP.getRTGWidth() << endl;
	cout << "ͼƬ�߶ȣ�       " << cVRP.getRTGHeight() << endl;
	cout << "�µ����߽��㣺   " << "(" << cVRP.getAim().lam << " , " << cVRP.getAim().phi << ")" << endl;

	cout << endl << "��ʼ�任�����Ժ�..." << endl;
	cVRP.outputYUV(".\\output.YUV");
	cout << endl << "�ɹ���" << endl;

	system("PAUSE");
	return 0;
}