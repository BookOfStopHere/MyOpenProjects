#include <iostream>
#include <string>

using namespace std;

//#include <opencv/highgui.h>

int main2 (int argc, char** argv) {
//	cout << "TestHZ \n"
//			<< "HZ 京 sizeof()=" << sizeof("京") << "\n"
//			<< "str A sizeof()=" << sizeof("A") << "\n"
//			<< "char A sizeof()=" << sizeof('A') << "\n";

	string str[2];
	str[0]="i miss you, 京A";
	str[1]=str[0];

	cout << str[0] << endl;
	cout << str[1] << endl; //输出一样的结果

	return 0;
}
