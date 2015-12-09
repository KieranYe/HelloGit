#include "ReadFile.h"

vector<float> readFile(string path) {
	ifstream infile(path);
	float a, b;
	char c;
	int num = 0;
	while (infile >> a >> c >> b)
	{
		num++;
	}
	vector<float> ret(2 * num);
	num = 0;
	ifstream infile2(path);
	while (infile2 >> a >> c >> b)
	{
		ret[num] = a;
		num++;
		ret[num] = b;
		num++;
	}
	return ret;
}