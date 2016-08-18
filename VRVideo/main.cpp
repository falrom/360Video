// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VRPicture.h"

int main(int argc, char *argv[])
{
	cout << endl;
	cout << "**************************************************************" << endl;
	cout << "**                     全景图片焦点变换                     **" << endl;
	cout << "**                         By 蒋昊                          **" << endl;
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

	cout << "YUV图片存放路径：" << cVRP.getYUVPath() << endl;
	cout << "图片宽度：       " << cVRP.getRTGWidth() << endl;
	cout << "图片高度：       " << cVRP.getRTGHeight() << endl;
	cout << "新的视线焦点：   " << "(" << cVRP.getAim().lam << " , " << cVRP.getAim().phi << ")" << endl;

	cout << endl << "开始变换，请稍后..." << endl;
	cVRP.outputYUV(".\\output.YUV");
	cout << endl << "成功！" << endl;

	system("PAUSE");
	return 0;
}