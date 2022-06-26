#include "viewer.h"
#include "Parser.h"
#include <sstream>	//istringstream  https://www.twblogs.net/a/5b8a17392b71775d1ce55f47
#include<Windows.h>
#include<string>

#include <QApplication>
#include <QDebug>

#define PI 3.1415926

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	Viewer w;
	w.show();
	return a.exec();
	/*
	cout << "enter 0 to open ui, 1 to test parser: ";
	int start; //std::cin >> start;
	start = 0;
	if (start == 0)
	{
		return runQt(argc, argv);
	}
	else
	{
		Parser a;
		QString now;
		a.test(QString("y = tan(x)"));
	}
	*/
}


/*
	test1
	Set Decimal a = -1.5	V
	Set Decimal b = a / 3	V
	y = a * b * x + a	


	test2
	Set Integer a = b + 1	V
	Set Integer y = a * x + c	
	y = x + ()	V
	y = sin(x	V
	Set Integer - c = 0		V

	test3
	y = cos(x) * 2
	y = sin(x) ^ cos(x)

	test4
	y = 3 * x ^ -2.5	
	y = x ^ 2 * 1.5 ^ 2 - 2		V

	extra 1
	y = tan(x)	 -V

	extra 2
	Set Decimal a=-1.5	V
	Set Decimal b=a/3	V
	y=a*b*x+a	V

	extra 3
	y ^ 4 = x	V

	extra 4
	x = 2 * y + 1	V

	extra 5
	x ^ 2 + 2 * y ^ 2 = 3	V
*/
