#include "Parser.h"
#include <algorithm>
#include<cmath>
#include "viewer.h"
using std::cout;
using std::string;
using std::istringstream;
using std::stringstream;
using std::vector;


#define PI 3.1415926
#define SCALE 20


std::vector<Parser> Parser::All;

std::string addition(std::string A, std::string B);
std::string subtraction(std::string A, std::string B);
Parser operation(Parser a, Parser b, string op);
int getPriority(char cmd);
Parser process(string s);

Parser::Parser(bool t, std::string n, std::string d)
{
	this->type = t;
	this->name = n;
	this->digits = d;
	this->dotPlace = digits.find('.');
	if (digits[0] == '-') {
		this->sign = 1;
		digits.erase(0, 1);
	}
	else {
		this->sign = 0;
	}
}

void Parser::test(QString now) {
	string line = now.toStdString();
	istringstream istr;
	string cmd;
	Parser temp;
	string to_process;

	//printAllVar();
	cout << "\n輸入運算式or設變數  :  \n";

		if (line.size() == 0)
			return;

		istr.clear();
		istr.str(line);
		istr >> cmd;

		if (cmd == "Set")
		{
			istr >> cmd;
			if (cmd == "Integer")
				temp.setType(0);

			else if (cmd == "Decimal")
				temp.setType(1);

			else
			{
				cout << "Illegal  :  未輸入正確 Integer or Decimal\n請重新輸入  :  ";
				type = -1; 
				return;
			}

			//read name (until '=')
			int equalPlace = line.find('=');
			if (equalPlace == string::npos)
			{
				cout << "Illegal  :  未輸入等號\n請重新輸入  :  ";
				type = -1; 
				return;
			}

			string name;
			name.assign(line, 12, equalPlace - 12);
			temp.setName(name);

			if (name[0] == '-') {
				cout << "Illegal  :  \n請重新輸入  :  ";
				type = -1; 
				return;
			}

			//set digits (after '=')
			to_process.clear();
			to_process.assign(line, equalPlace + 1, line.length());

			bool exist = false;		// 判斷變數是否已宣告
			for (int i = 0; i < to_process.length(); i++) {
				if (isalpha(to_process[i]) && to_process[i] != 'y' && to_process[i] != 'x') {
					for (int j = 0; j < Parser::All.size(); j++) {
						if (Parser::All[j].getName()[0] == to_process[i]) {
							exist = true;
						}
					}
				}
			}
			bool flag = true;		// 沒有變數為true
			for (int i = 0; i < to_process.length(); i++) {
				if (isalpha(to_process[i])) {
					flag = false;
				}
			}
			if (exist == false && flag == false) {
				cout << "Illegal  :  \n請重新輸入  :  ";
				type = -1;
				return;
			}
			else if (exist == true && flag == false) {		//代換已宣告的變數成數字
				for (int i = 0; i < to_process.length(); i++) {
					if (isalpha(to_process[i]) && to_process[i] != 'y' && to_process[i] != 'x') {
						for (int j = 0; j < Parser::All.size(); j++) {
							if (Parser::All[j].getName()[0] == to_process[i]) {
								cout << "Parser" << Parser::All[j].getDigits() << std::endl;
								to_process.erase(i, 1);
								to_process.insert(i, Parser::All[j].getDigits());
							}
						}
					}
				}
			}

			//cout << "to_process: " << to_process << std::endl;
			if (temp.getType() == 0)
			{
				temp = process(to_process);
				temp.setType(0);
			}
			else
			{
				temp = process(to_process);
				string sTemp = temp.getDigits();
				sTemp.insert(0, "-");
				temp.setDigits(sTemp);
				temp.setType(1);
			}
			temp.checkType();

			// erase 小數點多的零
			string finalDigits = temp.getDigits();
			int countZero = 0;
			for (int i = finalDigits.length() - 1; i >= 0; i--) {
				if (finalDigits[i] == '0') {
					countZero++;
				}
				else {
					break;
				}
			}
			finalDigits.erase(finalDigits.length() - countZero);
			temp.setDigits(finalDigits);

			bool check = 0;
			for (int i = 0; i < Parser::All.size(); i++)
			{
				if (Parser::All[i].getName() == temp.getName())
				{
					cout << temp.getName() << " already exist, change it to the new one.\n";
					Parser::All[i] = temp;
					check = 1;
					break;
				}
			}
			if (!check)
			{
				cout << "\nSet type: " << (temp.getType() == 0 ? "Integer" : "Decimal")
					<< "\nname: " << temp.getName()
					<< "\ndigits = " << temp.getDigits() << std::endl;
				Parser::All.push_back(temp);
			}
		}

		else
		{
			int tFunc = 0;		// if 三角函數計算 tFunc = 1
			int equalPlace = line.find('=');
			if (equalPlace == string::npos)
			{
				cout << "Illegal : 未輸入等號\n請重新輸入 : ";
				type = -1;
				return;
			}

			string leftTemp, rightTemp, left, right;		// 左式 右式
			if ((equalPlace == 1 || equalPlace == 2) && line[0] == 'y') {		// y = x
				left = "y";
			}
			else if ((equalPlace == 1 || equalPlace == 2) && line[0] == 'x') {
				left = "x";
			}
			else {		// y ^ 4 = x		// x ^ 2 + 2 * y ^ 2 = 3
				left.assign(line, 0, equalPlace);
			}
			right.assign(line, equalPlace + 1, line.length());


			bool flag = true;		// 沒有變數為true
			for (int i = 0; i < right.length(); i++) {
				if (isalpha(right[i])) {
					flag = false;
				}
			}

			if (flag == false) {		// y = x+a	// y = sin(x)
				bool exist = false;		// 判斷變數是否已宣告
				bool wrong = false;		// wrong case
				for (int i = 0; i < right.length(); i++) {
					if (right[i] == '(' && right[i + 1] == ')') {	// wrong case y = x + ()
						wrong = true;
						break;
					}
					if (isalpha(right[i]) && right[i] != 'x') {
						for (int j = 0; j < Parser::All.size(); j++) {
							if (Parser::All[j].getName()[0] == right[i]) {
								exist = true;
								break;
							}
						}
					}
					if ((right[i] == 's' && right[i + 1] == 'i' && right[i + 2] == 'n' && right[i + 3] == '(' && right[i + 5] == ')') ||
						(right[i] == 'c' && right[i + 1] == 'o' && right[i + 2] == 's' && right[i + 3] == '(' && right[i + 5] == ')') ||
						(right[i] == 't' && right[i + 1] == 'a' && right[i + 2] == 'n' && right[i + 3] == '(' && right[i + 5] == ')')) {
						exist = true;
						tFunc = 1;
						break;
					}
					if (right[i] == 'x' && i == right.length() - 1) {		// y=x
						exist = true;
						break;
					}
				}
				if (wrong == true) {
					cout << "Illegal  :  \n請重新輸入  :  ";
					type = -1;
					return;
				}
				bool chk = true;
				for (int i = 0; i < right.length(); i++) {
					if (right[i] == '(' && right[i + 1] == ')') {
						chk == false;
						break;
					}
					else if (right[i] != '+' && right[i] != '-' && right[i] != '*' && right[i] != '/' && right[i] != '^' &&
						right[i] != ' ' && right[i] != 'x' && !isdigit(right[i]))
					{
						chk == false;
					}
				}
				if (chk == true && exist == false) {
					// do nothing
				}
				else if (exist == false) {
					cout << "Illegal  :  \n請重新輸入  :  ";
					type = -1;
					return;
				}
				else {

				}
			}
			else {		// y=5
				if (left.find('x') != string::npos && left.find('y') != string::npos) {
					// x ^ 2 + 2 * y ^ 2 = 3
				}
				else {
					/*
					for (double i = -SCALE; i <= SCALE; i += 0.5) {
						for (double j = -SCALE; j <= SCALE; j += 0.5) {
							string s;
							stringstream ss;
							ss << i;
							ss >> s;
							if (right == s) {
								cout << "(" << j << ", " << i << ")" << std::endl;	// 傳到viewer畫圖
								x.push_back(j);
								y.push_back(i);
							}
						}
					}
					*/
					for (double i = -SCALE; i <= SCALE; i += 0.5) {
						double num;
						stringstream ss;
						ss << right;
						ss >> num;
						cout << "(" << i << ", " << num << ")" << std::endl; // 傳到viewer畫圖
						x.push_back(i);
						y.push_back(num);
					}
					cout << "[enter to continue]\n";
					getline(std::cin, line);

					//printAllVar();
					cout << "\n輸入運算式or設變數 : \n";
					return;
				}
			}

			// 處理左式的變數帶入
			for (int i = 0; i < left.length(); i++) {
				if (isalpha(left[i]) && left[i] != 'y') {
					for (int j = 0; j < Parser::All.size(); j++) {
						string var = "";
						var += left[i];
						if (Parser::All[j].getName() == var) {
							string num = Parser::All[j].getDigits();
							if (num[0] == '-') {
								num.insert(0, "(");
								num += ")";
							}
							left.erase(i, 1);
							left.insert(i, num);
						}
					}
				}
			}
			// 處理右式的變數帶入
			for (int i = 0; i < right.length(); i++) {
				if (isalpha(right[i]) && right[i] != 'x' &&
					(right[i] != 's' && right[i + 1] != 'i' && right[i + 2] != 'n') &&
					(right[i] != 'c' && right[i + 1] != 'o' && right[i + 2] != 's') &&
					(right[i] != 't' && right[i + 1] != 'a' && right[i + 2] != 'n')) {
					for (int j = 0; j < Parser::All.size(); j++) {
						string var = "";
						var += right[i];
						if (Parser::All[j].getName() == var) {
							string num = Parser::All[j].getDigits();
							if (num[0] == '-') {
								num.insert(0, "(");
								num += ")";
							}
							right.erase(i, 1);
							right.insert(i, num);
							for (int k = 0; k < right.length(); k++) {
								if (right[k] == var[0]) {
									right.erase(k, 1);
									right.insert(k, num);
								}
							}
						}
					}
				}
			}

			// change sin->S ; cos->C ; tan->T 
			bool stop = false;
			for (int i = 0; i < right.length(); i++) {
				if ((right[i] == 's' && right[i + 1] == 'i' && right[i + 2] == 'n' && (right[i + 3] != '(' || right[i + 5] != ')')) ||
					(right[i] == 'c' && right[i + 1] == 'o' && right[i + 2] == 's' && (right[i + 3] != '(' || right[i + 5] != ')')) ||
					(right[i] == 't' && right[i + 1] == 'a' && right[i + 2] == 'n' && (right[i + 3] != '(' || right[i + 5] != ')'))) {
					stop = true;		// wrong case y = sin(x
					break;
				}
				else if ((right[i] == 's' && right[i + 1] == 'i' && right[i + 2] == 'n' && right[i + 3] == '(' && right[i + 5] == ')') ||
					(right[i] == 'c' && right[i + 1] == 'o' && right[i + 2] == 's' && right[i + 3] == '(' && right[i + 5] == ')') ||
					(right[i] == 't' && right[i + 1] == 'a' && right[i + 2] == 'n' && right[i + 3] == '(' && right[i + 5] == ')')) {
					if (right[i] == 's') {
						right.erase(i, 3);
						right.insert(i, "S");
						right.erase(i + 1, 1);
						right.erase(i + 2, 1);
					}
					else if (right[i] == 'c') {
						right.erase(i, 3);
						right.insert(i, "C");
						right.erase(i + 1, 1);
						right.erase(i + 2, 1);
					}
					else {
						right.erase(i, 3);
						right.insert(i, "T");
						right.erase(i + 1, 1);
						right.erase(i + 2, 1);
					}
				}
			}
			if (stop == true) {
				cout << "Illegal  :  \n請重新輸入  :  ";
				type = -1;
				type = -1;
				return;
			}

			leftTemp = left;
			rightTemp = right;
			int posY = 0, posX = 0;
			int type = 0;	// type 1 means both x and y are in left

			if (right.find('x') == string::npos && right.find('y') == string::npos) {	// x ^ 2 + 2 * y ^ 2 = 3
				posY = left.find('y');
				posX = left.find('x');
				type = 1;
			}
			else if (leftTemp[0] == 'x') {
				posY = left.find('x');
				posX = right.find('y');
			}
			else {
				posY = left.find('y');
				posX = right.find('x');
			}

			if (tFunc == 1) {		// 三角函數
				cout << "\nAns: \n";
				for (double i = -SCALE; i <= SCALE; i += 0.5) {
					for (double j = -SCALE; j <= SCALE; j += 0.5) {
						string iString, jString;
						stringstream ss;
						ss << i;
						ss >> iString;
						ss.clear();
						ss << j;
						ss >> jString;

						if (type == 0) {
							leftTemp.erase(posY, 1);
							leftTemp.insert(posY, iString);
							rightTemp.erase(posX, 1);
							rightTemp.insert(posX, jString);
						}
						else {
							leftTemp.erase(posY, 1);
							leftTemp.insert(posY, iString);
							leftTemp.erase(posX, 1);
							leftTemp.insert(posX, jString);
						}

						if (leftTemp != "0") {
							rightTemp += "-(" + leftTemp + ')';
						}
						//cout << "rightTemp" << rightTemp << std::std::endl;

						//********* k range has to change *********
						for (int k = 0; k < 1; k++) {	// y=sin(x) -> S-10-(-10) -> -0.173648-(-10)
							cout << rightTemp;
							if (rightTemp[k] == 'S') {
								double 	sinValue = sin(j * PI / 180);
								stringstream ss;
								string value;
								ss << sinValue;
								ss >> value;
								int countErase = 1;
								int kTemp = k + 1;
								while ((rightTemp[kTemp] == '-' && rightTemp[kTemp + 1] != '(') || isdigit(rightTemp[kTemp])) {
									countErase++;
									kTemp++;
								}
								rightTemp.erase(k, countErase);
								rightTemp.insert(k, value);
							}
							else if (rightTemp[i] == 'C') {
								double 	cosValue = cos(j * PI / 180);
								stringstream ss;
								string value;
								ss << cosValue;
								ss >> value;
								int countErase = 1;
								int kTemp = k + 1;
								while ((rightTemp[kTemp] == '-' && rightTemp[kTemp + 1] != '(') || isdigit(rightTemp[kTemp])) {
									countErase++;
									kTemp++;
								}
								rightTemp.erase(k, countErase);
								rightTemp.insert(k, value);
							}
							else if (rightTemp[i] == 'T') {
								double 	tanValue = tan(j * PI / 180);
								stringstream ss;
								string value;
								ss << tanValue;
								ss >> value;
								int countErase = 1;
								int kTemp = k + 1;
								while ((rightTemp[kTemp] == '-' && rightTemp[kTemp + 1] != '(') || isdigit(rightTemp[kTemp])) {
									
									countErase++;
									kTemp++;
								}
								rightTemp.erase(k, countErase);
								rightTemp.insert(k, value);
							}
						}
						//cout << "rightTemp" << rightTemp << std::endl;

						temp = process(rightTemp);
						//cout << "temp" << temp << std::endl;
						if (temp == "0" || temp == "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") {
							cout << "(" << j << ", " << i << ")" << std::endl;	// 傳到viewer畫圖
							x.push_back(j);
							y.push_back(i);
						}

						leftTemp = left;
						rightTemp = right;
					}
				}
			}
			else {
				cout << "\nAns: \n";
				for (double i = -SCALE; i <= SCALE; i += 0.5) {
					for (double j = -SCALE; j <= SCALE; j += 0.5) {
						string iString, jString;
						stringstream ss;
						ss << i;
						ss >> iString;
						ss.clear();
						ss << j;
						ss >> jString;

						if (type == 0) {
							leftTemp.erase(posY, 1);
							leftTemp.insert(posY, iString);
							rightTemp.erase(posX, 1);
							rightTemp.insert(posX, jString);
						}
						else {		// x ^ 2 + 2 * y ^ 2 = 3
							leftTemp.erase(posY, 1);
							leftTemp.insert(posY, iString);
							leftTemp.erase(posX, 1);
							leftTemp.insert(posX, jString);
						}

						if (leftTemp != "0") {
							rightTemp += "-(" + leftTemp + ')';
						}
						//cout << "rightTemp" << rightTemp << std::endl;

						temp = process(rightTemp);
						//cout << "temp" << temp << std::endl;
						if (temp == "0" || temp == "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") {
							cout << "(" << j << ", " << i << ")" << std::endl;	// 傳到viewer畫圖
							x.push_back(j);
							y.push_back(i);
						}

						leftTemp = left;
						rightTemp = right;
					}
				}
			}
		}
}

Parser Parser::operator+(const Parser rhs) const
{
	Parser c;
	bool ansType;
	bool ansSign;
	std::string ansDigits = "";
	int ansDotPlace;

	if (sign == 0 && rhs.sign == 0) {		// 正+正
		ansDigits = addition(digits, rhs.digits);
		ansSign = 0;

	}
	else if (sign == 1 && rhs.sign == 1) {		// 負+負
		std::string a = digits;
		//a.erase(0, 1);
		std::string b = rhs.digits;
		//b.erase(0, 1);
		ansDigits = addition(a, b);		// 以正+正去運算
		//ansDigits = '-' + ansDigits;		// 把負號加回來
		ansSign = 1;
	}
	else {
		if (sign == 0 && rhs.sign == 1) {		// 正+負 = 正-正
			ansDigits = subtraction(digits, rhs.digits);
		}
		else {		//sign == 1 && rhs.sign == 0		//負+正 = 正-正
			ansDigits = subtraction(rhs.digits, digits);
		}
		if (ansDigits[0] == '-') {
			ansSign = 1;
			ansDigits.erase(0, 1);
		}
		else {
			ansSign = 0;
		}
	}
	ansDotPlace = ansDigits.find('.');
	if (ansDotPlace != std::string::npos) {
		ansType = 1;
	}
	else {
		ansType = 0;
	}

	c.type = ansType;
	c.sign = ansSign;
	c.digits = ansDigits;
	c.dotPlace = ansDotPlace;

	return c;
}

Parser Parser::operator-(const Parser rhs) const
{
	Parser c;
	bool ansType;
	bool ansSign;
	std::string ansDigits = "";
	int ansDotPlace;

	if (sign == 0 && rhs.sign == 0) {		// 正-正
		ansDigits = subtraction(digits, rhs.digits);
		if (ansDigits[0] == '-') {
			ansSign = 1;
			ansDigits.erase(0, 1);
		}
		else {
			ansSign = 0;
		}
		ansDotPlace = ansDigits.find('.');
		if (ansDotPlace != std::string::npos) {
			ansType = 1;
		}
		else {
			ansType = 0;
		}
	}
	else if (sign == 1 && rhs.sign == 1) {		// 負-負 = 負(正-正)
		std::string a = digits;
		//a.erase(0, 1);
		std::string b = rhs.digits;
		//b.erase(0, 1);
		ansDigits = subtraction(a, b);		// 以正-正去運算
		if (ansDigits[0] == '-') {
			ansDigits.erase(0, 1);			// 把負號去掉
			ansSign = 0;
		}
		else {
			//ansDigits = '-' + ansDigits;		// 把負號加回來
			ansSign = 1;
		}

		ansDotPlace = ansDigits.find('.');
		if (ansDotPlace != std::string::npos) {
			ansType = 1;
		}
		else {
			ansType = 0;
		}
	}
	else {
		std::string a = digits;
		if (a[0] == '-')
			a.erase(0, 1);
		std::string b = rhs.digits;
		if (b[0] == '-')
			b.erase(0, 1);

		if (sign == 0 && rhs.sign == 1) {		// 正-負 = 正+正
			ansDigits = addition(a, b);
			ansSign = 0;
		}
		else {		//sign == 1 && rhs.sign == 0		//負-正 = 負(正+正)
			ansDigits = addition(a, b);
			//ansDigits = '-' + ansDigits;
			ansSign = 1;
		}

		ansDotPlace = ansDigits.find('.');
		if (ansDotPlace != std::string::npos) {
			ansType = 1;
		}
		else {
			ansType = 0;
		}
	}
	if (c.digits[0] == '.')
		c.digits = '0' + c.digits;
	c.type = ansType;
	c.sign = ansSign;
	c.digits = ansDigits;
	c.dotPlace = ansDotPlace;

	return c;
}

Parser Parser::operator*(const Parser rhs) const
{
	Parser c;
	std::string ans;
	int decPt;
	int len1 = digits.size();
	int len2 = rhs.digits.size();
	// 101 0s
	std::string dec_0 = "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	// judge ans int or dec
	if (type == 0 && rhs.type == 0) {
		decPt = -1;
		c.type = false;
	}
	else {
		c.type = true;

		if (dotPlace == std::string::npos) {
			decPt = digits.length() + rhs.dotPlace - 1;
		}

		else if (rhs.dotPlace == std::string::npos)
			decPt = rhs.digits.length() + dotPlace - 1;

		else
			decPt = dotPlace + rhs.dotPlace;//0.01*0.0123
	}

	if (this->compare("0") == 0 || rhs.compare("0") == 0)
		if (c.type == false)
			ans = "0";
		else
			ans = dec_0;


	else
	{
		if (type && rhs.type)
		{
			len1 -= 50;
			len2 -= 50;
		}

		//vector <int> res(len1 + len2, 0); //  - ((type == 1) ? 1 : 0) - ((rhs.type == 1) ? 1 : 0)
		std::vector <int> res(len1 + len2, 0);
		int _1 = 0;
		int _2 = 0;
		for (int i = len1 - 1; i >= 0; i--)
		{
			if (digits[i] == '.') continue;

			int cry = 0;
			int n1 = digits[i] - '0';

			// shift pos to l after every multiplication of a digit in rhs
			_2 = 0;
			// go from right to left in rhs
			for (int j = len2 - 1; j >= 0; j--)
			{
				if (rhs.digits[j] == '.') continue;
				// take current digit of second number
				int n2 = rhs.digits[j] - '0';
				// multiply with current digit of 1st num & add res to prev stored res at current pos
				int sum = n1 * n2 + res[_1 + _2] + cry;
				// carry for next iteration
				cry = sum / 10;
				res[_1 + _2] = sum % 10;
				_2++;
			}
			// store carry in next cell
			if (cry > 0)
				res[_1 + _2] += cry;
			// shift pos to left after every multiplication of a digit og       
			_1++;
		}


		// ignore 0s from the right
		//digits.length()-digits.dotPlace = 小數長度
		int q = res.size() - 1;
		if (c.type == 1)
		{
			while (q > 100 && res[q] == 0)
				q--;
			while (q >= 100)
				ans += res[q--] + '0';
			ans += '.';
			while (q >= 0)
				ans += res[q--] + '0';
		}
		else
		{
			while (q >= 0 && res[q] == 0)
				q--;
			while (q >= 0)
				ans += std::to_string(res[q--]);
		}

		//進位 overflow

		if (ans.find('.') != std::string::npos)
		{
			if (std::count(res.begin(), res.begin() + 100, 9) > 50)
			{
				ans = addition(ans, "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
				for (int i = ans.find('.') + 1; i < ans.length(); i++)
					ans[i] = '0';
			}
		}

		//for (int i = 0; i < res.size(); i++) cout << "[" << i << "] " << res[i] << endl;
	}



	c.setSign(!(sign == rhs.sign));
	c.setDigits(ans);
	c.setDotPlace(c.digits.find('.'));
	return c;

}

//  (1 / 3) * 3 = 1 -> 先乘後除(process 裡面判斷)
Parser Parser::operator/(const Parser rhs) const	// (digit - '0') / (rhs.digit - '0') -> A / B
{
	Parser c;
	c.digits = "0";

	Parser remain = *this; remain.sign = 0;
	Parser divisor = rhs;  divisor.sign = 0;

	Parser ten;
	ten.digits = "10";
	ten.sign = 0;
	ten.type = 0;
	ten.dotPlace = std::string::npos;

	c.setSign(!(sign == rhs.sign));	// 正負數
	c.setType(type || rhs.type);

	if (rhs.compare("0") == 0)
	{
		c.digits = "default";		// 錯誤
		return c;
	}
	if (this->compare("0") == 0)
	{
		return c;
	}

	while (remain.compare(divisor.digits) > -1) // A >= B
	{
		remain = remain - divisor;
		c.digits = addition(c.digits, "1");
	}

	if (c.type == 0)
		return c;
	c.digits += '.';		// now ansDigits = 52.
	int now;
	for (int i = 0; i < 100; i++)	//做到ansDigits小數有一百位
	{
		now = 0;
		remain = remain * ten;
		//123.456/0.450
		while (remain.compare(divisor.digits) > -1)
		{
			remain = remain - divisor;
			now++;
		}
		c.digits += now + '0';
	}

	c.dotPlace = c.digits.find('.');

	return c;
}

Parser Parser::operator^(Parser rhs) const
{
	Parser one;
	one.digits = "1";
	one.sign = 0;
	one.type = 0;
	one.dotPlace = std::string::npos;

	Parser ans = one;
	//b 只有可能是 0.5 的整數倍

	while (rhs.compare("1") >= 0)
	{
		ans = ans * *this;
		rhs = rhs - one;
	}

	if (rhs.compare("0.5") == 0)
	{
		Parser two = one + one;	two.type = 1; two.checkType();
		Parser guess = *this / two;//guess will be equal to *this ^ 0.5
		Parser r;
		Parser temp;

		do {
			r = *this / guess;
			guess = (guess + r) / two;
			//cout << guess << endl;
			temp = r - guess;
			if (temp.sign == 1)temp.sign = 0;
		} while (temp.compare("0.00000000000000001") >= 0);

		ans = ans * guess;
	}

	return ans;
}

std::string addition(std::string A, std::string B)		// 都會是正+正 (但可能有小數)
{
	int dot1 = A.find('.');
	int dot2 = B.find('.');
	int carry = 0;
	int len;
	int dot;
	int base = '0';

	if (dot1 == std::string::npos && dot2 == std::string::npos)		// 整數 + 整數
	{
		dot = dot1;
		// 補齊整數
		while (A.length() < B.length())
			A = '0' + A;

		while (B.length() < A.length())
			B = '0' + B;

	}
	else if (dot1 == std::string::npos) //整數 + 小數
	{
		dot = (dot2 > A.length()) ? dot2 : A.length();
		// 補齊整數
		while (dot2 < dot)
		{
			B = '0' + B;
			dot2++;
		}
		while (A.length() < dot)
			A = '0' + A;

		//補齊小數
		A += '.';
		while (A.length() < B.length())
			A += '0';

	}
	else if (dot2 == std::string::npos) //小數 + 整數
	{
		dot = (dot1 > B.length()) ? dot1 : B.length();
		// 補齊整數
		while (dot1 < dot)
		{
			A = '0' + A;
			dot1++;
		}
		while (B.length() < dot)
			B = '0' + B;

		//補齊小數
		B += '.';
		while (B.length() < A.length())
			B += '0';
	}
	else 								// 小數 + 小數 eg: 123.3457 + 67.891
	{
		dot = std::max(dot1, dot2);
		//補齊整數
		while (dot1 < dot2)
		{
			A = '0' + A;
			dot1++;
		}
		while (dot2 < dot1)
		{
			B = '0' + B;
			dot2++;
		}
		//補齊小數
		while (A.length() < B.length())
			A += '0';

		while (B.length() < A.length())
			B += '0';
	}

	len = A.length();	//A.len == B.len，所以不用比較
	int temp;
	if (dot != std::string::npos)
	{
		for (int i = len - 1; i > dot; i--)//小數部分
		{
			temp = A[i] + B[i] - 2 * base + carry;
			carry = temp / 10;
			A[i] = temp % 10 + base;
		}
	}
	else
		dot = len;

	for (int i = dot - 1; i >= 0; i--)//整數部分
	{
		temp = A[i] + B[i] - 2 * base + carry;
		carry = temp / 10;
		A[i] = temp % 10 + base;
	}

	if (carry == 1) {
		A = '1' + A;
	}

	return A;
}

std::string subtraction(std::string A, std::string B)		// 都會是正-正 (但可能有小數)
{
	int dot1 = A.find('.');
	int dot2 = B.find('.');
	int dot;
	int base = '0';
	std::string ans = "";
	char ansSign;

	if (dot1 != std::string::npos && dot2 != std::string::npos)	//小數 - 小數
	{
		dot = std::max(dot1, dot2);
		//補齊整數
		while (dot1 < dot2)
		{
			A = '0' + A;
			dot1++;
		}
		while (dot2 < dot1)
		{
			B = '0' + B;
			dot2++;
		}
		//補齊小數
		while (A.length() < B.length())
			A += '0';
		while (B.length() < A.length())
			B += '0';
	}
	else if (dot1 == std::string::npos && dot2 == std::string::npos)		// 整數 - 整數
	{
		while (A.length() < B.length())
			A = '0' + A;
		while (B.length() < A.length())
			B = '0' + B;
	}
	else if (dot1 == std::string::npos)	// 整數 - 小數
	{
		dot = (dot2 > A.length()) ? dot2 : A.length();
		// 補齊整數
		while (dot2 < dot)
		{
			B = '0' + B;
			dot2++;
		}
		while (A.length() < dot)
			A = '0' + A;

		//補齊小數
		A += '.';
		while (A.length() < B.length())
			A += '0';
	}
	else								// 小數 - 整數
	{
		dot = (dot1 > B.length()) ? dot1 : B.length();
		// 補齊整數
		while (dot1 < dot)
		{
			A = '0' + A;
			dot1++;
		}
		while (B.length() < dot)
			B = '0' + B;

		//補齊小數
		B += '.';
		while (B.length() < A.length())
			B += '0';
	}

	//比較AB
	ansSign = '+';

	for (int i = 0; i < A.length(); i++)
	{
		if (A[i] > B[i])
			break;
		if (A[i] < B[i]) {
			ansSign = '-';
			A.swap(B);
			break;
		}
	}

	dot = std::max(dot1, dot2);
	// now A > B
	int len1 = A.length() - 1;
	int borrow = 0;
	ans = A;
	for (int i = len1; i > dot; i--) {
		ans[i] = ans[i] - B[i] - borrow + '0';

		if (ans[i] < '0') {
			borrow = 1;
			ans[i] = ans[i] + 10;
		}
		else {
			borrow = 0;
		}
	}
	for (int i = dot - 1; i >= 0; i--) {
		ans[i] = ans[i] - B[i] - borrow + '0';

		if (ans[i] < '0') {
			borrow = 1;
			ans[i] = ans[i] + 10;
		}
		else {
			borrow = 0;
		}
	}

	while (ans[0] == '0')
		ans.erase(0, 1);

	if (ans.length() == 0 || ans[0] == '.')
		ans = '0' + ans;

	if (ansSign == '-') {
		ans = '-' + ans;
	}
	return ans;
}

void Parser::operator=(const Parser& rhs)
{
	type = rhs.type;
	//name不用改
	digits = rhs.digits;
	dotPlace = rhs.dotPlace;
	sign = rhs.sign;
}

int Parser::compare(std::string B) const
{
	//回傳: -1:<  0:==  1:>

	//if sign is different
	if (B[0] == '-')
	{
		if (sign == 0)
			return 1;
		B.erase(0, 1);//消去負號，以便後面判斷
	}
	else if (sign == 1)
		return -1;

	std::string A = digits;
	int dot1 = A.find('.'),
		dot2 = B.find('.');

	if (dot1 == std::string::npos)
	{
		dot1 = A.length();
		A += ".0";
	}
	if (dot2 == std::string::npos)
	{
		dot2 = B.length();
		B += ".0";
	}
	//補齊整數
	while (dot1 < dot2)
	{
		A = '0' + A;
		dot1++;
	}
	while (dot2 < dot1)
	{
		B = '0' + B;
		dot2++;
	}
	//補齊小數
	while (A.length() < B.length())
		A += '0';

	while (B.length() < A.length())
		B += '0';

	//start compare
	for (int i = 0; i < A.length(); i++)
	{
		if (A[i] < B[i])
			return -1;
		else if (A[i] > B[i])
			return 1;
	}
	return 0;
}

std::ostream& operator<<(std::ostream& strm, const Parser& now)
{
	//if ansDigits = default -> 分母為0

	strm << (now.sign ? '-' : ' ');
	strm << now.digits;
	return strm;
}

bool Parser::operator==(std::string zero)
{
	if (digits == "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") {
		return true;
	}
	if (digits == zero) {
		return true;
	}
	return false;
}

Parser operation(Parser a, Parser b, string op)
{
	if (op == "+")
		return a + b;

	else if (op == "-")
		return a - b;

	else if (op == "*")
		return a * b;

	else if (op == "/")
		return a / b;


	cout << "operation error\n";
	return a;
}


int getPriority(char cmd)
{
	switch (cmd)
	{
	case ','://Power(a,b)
		return 1;
	case '+'://**加減 & 正負號 的區分?
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	case '^':
		return 4;
	case '!':
		return 5;
	case '(':
	case ')':
		return 6;
	default:
		return 0;
	}
}


Parser process(string s)
{
	string infix;
	//清除空格
	//infix.erase(std::remove(infix.begin(), infix.end(), ' '), infix.end());//remove space ' '
	for (int i = 0; i < s.length(); i++)
		if (s[i] != ' ')
			infix += s[i];

	//-------------------------------------------
	// 中敘式 -> 後序式 <Infix to Postfix>
	//-------------------------------------------
	Parser nowVar;//an number to save into stack
	nowVar.sign = 0;
	std::vector<Parser> postfix, stack;
	Parser now;//an operation to save into stack
	now.sign = 0;
	now.type = 2;
	bool isNegative = false;	//區別負號&減號用	
	bool doPower = false;
	bool check = false;

	for (int i = 0; i < infix.length(); i++)
	{
		switch (infix[i])
		{
		case'(':
			now.digits = "(";
			if (isNegative)
			{
				now.sign = 1;
				isNegative = 0;
			}
			else now.sign = 0;
			stack.push_back(now);
			break;

		case')': //一直做到整個括號包完
			while (stack.size() > 0)
			{
				if (stack.back().digits == "(")
				{
					stack.pop_back();
					break;
				}
				else
				{
					postfix.push_back(stack.back());
					stack.pop_back();
				}
			}
			break;

		default:
			int priority = getPriority(infix[i]);
			if (priority > 0)	//不是數字
			{
				if (infix[i] == '-' && (i == 0 || getPriority(infix[i - 1]) != 0)) //前一個不是數字 or 這在式子最前面，這就是負號
				{
					if (i > 2 && infix[i - 1] != '(' && infix[i - 2] != '(') {
						break;
					}

					if (isNegative)
						isNegative = 0;
					else
						isNegative = 1;
					break;	//讓負號不會在stack裡面，判斷stack時才不會混淆負號&減號
				}

				while (stack.size() > 0)
				{
					if (stack.back().digits == "(")
						break;

					if (priority > getPriority(stack.back().digits[0]))
						break;

					if (priority == 4)//階乘是由右至左運算(2^3^4 == 2^(3^4))
						break;

					postfix.push_back(stack.back());
					stack.pop_back();
				}
				now.type = 2;
				now.digits.clear();
				now.digits = infix[i];
				stack.push_back(now);
				//cout << "put stack :" << now.digits << std::endl;
			}
			else
			{
				if (i + 1 < infix.length() && getPriority(infix[i + 1]) == 0) //下一格也是同個數字 or 英文
					nowVar.digits.push_back(infix[i]);

				else { //下一格不是數字 or 英文
					nowVar.digits.push_back(infix[i]);
					if (nowVar.digits[0] > '9' || nowVar.digits[0] < '0')//是一個變數名稱 or Power
					{
						if (nowVar.digits[0] == 'P' && nowVar.digits[1] == 'o' && nowVar.digits[2] == 'w' && nowVar.digits[3] == 'e' && nowVar.digits[4] == 'r')
						{
							//doPower = true;
							nowVar.digits.clear();

							break;
						}
						for (int i = 0; i < Parser::All.size(); i++)
						{
							if (Parser::All[i].name == nowVar.digits)
							{
								nowVar = Parser::All[i];
								break;
							}
						}
					}
					else
					{
						nowVar.dotPlace = nowVar.digits.find('.');
						if (nowVar.dotPlace == string::npos) //是整數
							nowVar.type = 0;
						else
							nowVar.type = 1;

						nowVar.checkType();
					}
					if (check) break;

					if (isNegative)
					{
						if (nowVar.sign)
							nowVar.sign = 0;
						else
							nowVar.sign = 1;

						isNegative = false;
					}
					postfix.push_back(nowVar);
					//cout << "putNowVar: " << (nowVar.sign ? '-' : ' ') << nowVar.digits << " ( dot at " << nowVar.dotPlace << " )" << std::endl;
					nowVar.digits.clear();
					nowVar.sign = 0;
				}
			}
			break;
		}

		/*
		cout << std::endl << "stack: ";
		for (int i = 0; i < stack.size(); i++)
			cout << (stack[i].sign ? '-' : ' ') << stack[i].digits << ",\n";
		cout << std::endl << "out  : ";
		for (int i = 0; i < postfix.size(); i++)
			cout << (postfix[i].sign ? '-' : ' ') << postfix[i].digits << ",\n";
		*/
	}

	while (stack.size() > 0)
	{
		postfix.push_back(stack.back());
		stack.pop_back();
	}

	//return postfix[];
	/*cout << "\n-------------------final  postfix-------------------\n\n";
	for (int i = 0; i < postfix.size(); i++)
		cout << (postfix[i].sign ? '-' : ' ') << postfix[i].digits << ",\n";
	cout << "\n----------------------------------------------------\n";*/


	//cout << "\n-------------------- [運算進行] --------------------\n";
	//-------------------------------------------
	// 算後序式 <Evaluate Postfix>
	//-------------------------------------------
  /*
	for (int i = 2; i < postfix.size(); i++) {
		if (postfix[i].digits == "/" && postfix[i + 2].digits == "*" && postfix[i - 2].digits != "(") {
			swap(postfix[i].digits, postfix[i + 2].digits);
			swap(postfix[i - 1].digits, postfix[i + 1].digits);
			break;
		}
	}
*/
	for (int i = 0; i < postfix.size(); i++)
	{
		if ((postfix[i].digits == "+") || (postfix[i].digits == "-") || (postfix[i].digits == "*") || (postfix[i].digits == "/") || (postfix[i].digits == "," || (postfix[i].digits == "^")))
		{
			//運算
			Parser b = stack.back();
			stack.pop_back();
			Parser a = stack.back();
			stack.pop_back();
			//cout << "operation : " << a.digits << " " << postfix[i].digits << " " << b.digits << " = " << operation(a, b, postfix[i].digits).digits << std::endl;
			//stack.push_back(operation(a, b, postfix[i].digits));

			if (postfix[i].digits == "+")
				stack.push_back(a + b);

			else if (postfix[i].digits == "-")
				stack.push_back(a - b);

			else if (postfix[i].digits == "*")
				stack.push_back(a * b);

			else if (postfix[i].digits == "/")
				stack.push_back(a / b);

			else if ((postfix[i].digits == "^") || (postfix[i].digits == ","))
				stack.push_back(a ^ b);

			else cout << "operation error\n";

			if (postfix[i].sign == 1)
			{
				if (stack.back().sign == 1)
					stack.back().sign = 0;
				else stack.back().sign = 1;
			}

			if (stack.back().digits == "default")
			{
				stack.back().sign = 0;
				return stack.back();
			}
			if (stack.back().digits == "0")
			{
				stack.back().sign = 0;
			}

		}
		else if (postfix[i].digits == "!")
		{
			Parser a = stack.back();
			stack.pop_back();
			if (a.type != 0 || a.sign != 0) {
				for (int i = a.digits.find('.') + 1; i < a.digits.length(); i++)
				{
					if (a.digits[i] != '0')
					{
						cout << "Illegal : 階乘運算須為或正整數\n請重新輸入 : ";
						a.digits = "default";
						a.sign = 0;
						return a;
					}
				}
			}

			Parser one;
			one.digits = "1"; one.type = 0; one.sign = 0;
			if (a.compare("0") == 0) // 0!
				stack.push_back(one);
			else
			{
				Parser times = a - one;
				while (times.compare("1") == 1) //a > 1
				{
					a = a * times;
					times = times - one;
				}
				stack.push_back(a);
			}
		}
		else
		{
			stack.push_back(postfix[i]);
		}

		/*
		cout << std::endl << "stack: ";
		for (int i = 0; i < stack.size(); i++)
			cout << (stack[i].sign ? '-' : ' ') << stack[i].digits << ", ";
		cout << std::endl << "postfix  : ";
		for (int i = 0; i < postfix.size(); i++)
			cout << (postfix[i].sign ? '-' : ' ') << postfix[i].digits << ", ";
		*/
	}

	if (stack.back().getDigits().find('.') == string::npos) //是整數
		stack.back().setType(0);
	else
		stack.back().setType(1);

	stack.back().checkType();
	return stack.back();
}

