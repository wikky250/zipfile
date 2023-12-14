// zipdemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iomanip>
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <fstream>
#include <bit7z/bitfilecompressor.hpp>
#include <bit7z/bitarchivereader.hpp>

using namespace std;
using namespace bit7z;
uint64_t g_iTotalSize = 0;
std::string filetoZip;
std::string filetoDst;
bool totalCallBack(uint64_t proindex)
{
	g_iTotalSize = proindex;
	return true;
}

bool progressCallback(uint64_t proindex)
{
	if (g_iTotalSize)
	{
		//if((proindex * 1.0 / g_iTotalSize)%0.1==0)
		std::cout << setiosflags(ios::fixed) << std::setprecision(1);
		std::cout << proindex * 1.0 / g_iTotalSize * 100.0 << "%" << "\r";
	}
	return true;
}
bool isFileExist(std::string fname)
{
	fstream fs;
	fs.open(fname, ios::in);

	if (!fs)
	{
		return true;
	}
	else
	{
		fs.close();
		if (remove(fname.c_str()) == 0)
		{
			cout << "Delete Success" << endl;
		}
		else
		{
			cout << "Delete Error:"<< fname << endl;
		}
		return true;
	}
	return true;
}
int main(int argc, char* argv[])
{
	using namespace bit7z;
	if (argc == 3)
	{
		filetoZip = argv[1];
		filetoDst = argv[2];
	}
	else
	{
		std::cout << "Input Param less 2" << std::endl;
		return 0;
	}
	isFileExist(filetoDst);

	std::future<void> result = std::async(
		std::launch::async, [=]()
		{
			try
			{
				std::cout << "Zipping" << std::endl;
				Bit7zLibrary lib{ "7z.dll" };
				BitFileCompressor compressor{ lib, BitFormat::SevenZip };
				compressor.setCompressionLevel(BitCompressionLevel::Normal);
				compressor.setUpdateMode(UpdateMode::Update);
				compressor.setThreadsCount(4);
				compressor.setProgressCallback(std::bind(progressCallback, std::placeholders::_1));
				compressor.setTotalCallback(std::bind(totalCallBack, std::placeholders::_1));
				compressor.compressDirectory(filetoZip, filetoDst);
				std::cout << "Finish zip file: " << filetoDst;
			}
			catch (const bit7z::BitException& ex) {
				std::cout << ex.what() << std::endl;
			}
		}
	);
}