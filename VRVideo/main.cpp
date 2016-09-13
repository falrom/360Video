// main.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VRPicture.h"

/**
> 主函数。
- 配置文件路径。
- 需要转换几帧
*/
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

	int numOfFrames = atoi(argv[2]);
	double step = (double)atoi(argv[3]);

	cout << "YUV存放路径：	" << cVRP.getYUVPath() << endl;
	cout << "图片宽度：		" << cVRP.getRTGWidth() << endl;
	cout << "图片高度：		" << cVRP.getRTGHeight() << endl;
	//cout << "新的视线焦点：   " << "(" << cVRP.getAim().lam << " , " << cVRP.getAim().phi << ")" << endl;

	//cout << endl << "开始变换，请稍后..." << endl;
	//cVRP.outputYUV(".\\output.yuv");
	//cout << endl << "成功！" << endl;

	//cout << endl << "开始生成全景播放器视角，请稍后..." << endl;
	//cVRP.perspectiveYUV(".\\show.yuv");
	//cout << endl << "成功！" << endl;

	//cout << endl << "测试中..." << endl;
	//cVRP.outputOrigYUV();
	//cout << endl << "测试完毕。" << endl;

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