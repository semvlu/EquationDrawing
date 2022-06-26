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
	cout << "\n��J�B�⦡or�]�ܼ�  :  \n";

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
				cout << "Illegal  :  ����J���T Integer or Decimal\n�Э��s��J  :  ";
				type = -1; 
				return;
			}

			//read name (until '=')
			int equalPlace = line.find('=');
			if (equalPlace == string::npos)
			{
				cout << "Illegal  :  ����J����\n�Э��s��J  :  ";
				type = -1; 
				return;
			}

			string name;
			name.assign(line, 12, equalPlace - 12);
			temp.setName(name);

			if (name[0] == '-') {
				cout << "Illegal  :  \n�Э��s��J  :  ";
				type = -1; 
				return;
			}

			//set digits (after '=')
			to_process.clear();
			to_process.assign(line, equalPlace + 1, line.length());

			bool exist = false;		// �P�_�ܼƬO�_�w�ŧi
			for (int i = 0; i < to_process.length(); i++) {
				if (isalpha(to_process[i]) && to_process[i] != 'y' && to_process[i] != 'x') {
					for (int j = 0; j < Parser::All.size(); j++) {
						if (Parser::All[j].getName()[0] == to_process[i]) {
							exist = true;
						}
					}
				}
			}
			bool flag = true;		// �S���ܼƬ�true
			for (int i = 0; i < to_process.length(); i++) {
				if (isalpha(to_process[i])) {
					flag = false;
				}
			}
			if (exist == false && flag == false) {
				cout << "Illegal  :  \n�Э��s��J  :  ";
				type = -1;
				return;
			}
			else if (exist == true && flag == false) {		//�N���w�ŧi���ܼƦ��Ʀr
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

			// erase �p���I�h���s
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
			int tFunc = 0;		// if �T����ƭp�� tFunc = 1
			int equalPlace = line.find('=');
			if (equalPlace == string::npos)
			{
				cout << "Illegal : ����J����\n�Э��s��J : ";
				type = -1;
				return;
			}

			string leftTemp, rightTemp, left, right;		// ���� �k��
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


			bool flag = true;		// �S���ܼƬ�true
			for (int i = 0; i < right.length(); i++) {
				if (isalpha(right[i])) {
					flag = false;
				}
			}

			if (flag == false) {		// y = x+a	// y = sin(x)
				bool exist = false;		// �P�_�ܼƬO�_�w�ŧi
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
					cout << "Illegal  :  \n�Э��s��J  :  ";
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
					cout << "Illegal  :  \n�Э��s��J  :  ";
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
								cout << "(" << j << ", " << i << ")" << std::endl;	// �Ǩ�viewer�e��
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
						cout << "(" << i << ", " << num << ")" << std::endl; // �Ǩ�viewer�e��
						x.push_back(i);
						y.push_back(num);
					}
					cout << "[enter to continue]\n";
					getline(std::cin, line);

					//printAllVar();
					cout << "\n��J�B�⦡or�]�ܼ� : \n";
					return;
				}
			}

			// �B�z�������ܼƱa�J
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
			// �B�z�k�����ܼƱa�J
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
				cout << "Illegal  :  \n�Э��s��J  :  ";
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

			if (tFunc == 1) {		// �T�����
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
							cout << "(" << j << ", " << i << ")" << std::endl;	// �Ǩ�viewer�e��
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
							cout << "(" << j << ", " << i << ")" << std::endl;	// �Ǩ�viewer�e��
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

	if (sign == 0 && rhs.sign == 0) {		// ��+��
		ansDigits = addition(digits, rhs.digits);
		ansSign = 0;

	}
	else if (sign == 1 && rhs.sign == 1) {		// �t+�t
		std::string a = digits;
		//a.erase(0, 1);
		std::string b = rhs.digits;
		//b.erase(0, 1);
		ansDigits = addition(a, b);		// �H��+���h�B��
		//ansDigits = '-' + ansDigits;		// ��t���[�^��
		ansSign = 1;
	}
	else {
		if (sign == 0 && rhs.sign == 1) {		// ��+�t = ��-��
			ansDigits = subtraction(digits, rhs.digits);
		}
		else {		//sign == 1 && rhs.sign == 0		//�t+�� = ��-��
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

	if (sign == 0 && rhs.sign == 0) {		// ��-��
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
	else if (sign == 1 && rhs.sign == 1) {		// �t-�t = �t(��-��)
		std::string a = digits;
		//a.erase(0, 1);
		std::string b = rhs.digits;
		//b.erase(0, 1);
		ansDigits = subtraction(a, b);		// �H��-���h�B��
		if (ansDigits[0] == '-') {
			ansDigits.erase(0, 1);			// ��t���h��
			ansSign = 0;
		}
		else {
			//ansDigits = '-' + ansDigits;		// ��t���[�^��
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

		if (sign == 0 && rhs.sign == 1) {		// ��-�t = ��+��
			ansDigits = addition(a, b);
			ansSign = 0;
		}
		else {		//sign == 1 && rhs.sign == 0		//�t-�� = �t(��+��)
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
		//digits.length()-digits.dotPlace = �p�ƪ���
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

		//�i�� overflow

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

//  (1 / 3) * 3 = 1 -> �����ᰣ(process �̭��P�_)
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

	c.setSign(!(sign == rhs.sign));	// ���t��
	c.setType(type || rhs.type);

	if (rhs.compare("0") == 0)
	{
		c.digits = "default";		// ���~
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
	for (int i = 0; i < 100; i++)	//����ansDigits�p�Ʀ��@�ʦ�
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
	//b �u���i��O 0.5 ����ƭ�

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

std::string addition(std::string A, std::string B)		// ���|�O��+�� (���i�঳�p��)
{
	int dot1 = A.find('.');
	int dot2 = B.find('.');
	int carry = 0;
	int len;
	int dot;
	int base = '0';

	if (dot1 == std::string::npos && dot2 == std::string::npos)		// ��� + ���
	{
		dot = dot1;
		// �ɻ����
		while (A.length() < B.length())
			A = '0' + A;

		while (B.length() < A.length())
			B = '0' + B;

	}
	else if (dot1 == std::string::npos) //��� + �p��
	{
		dot = (dot2 > A.length()) ? dot2 : A.length();
		// �ɻ����
		while (dot2 < dot)
		{
			B = '0' + B;
			dot2++;
		}
		while (A.length() < dot)
			A = '0' + A;

		//�ɻ��p��
		A += '.';
		while (A.length() < B.length())
			A += '0';

	}
	else if (dot2 == std::string::npos) //�p�� + ���
	{
		dot = (dot1 > B.length()) ? dot1 : B.length();
		// �ɻ����
		while (dot1 < dot)
		{
			A = '0' + A;
			dot1++;
		}
		while (B.length() < dot)
			B = '0' + B;

		//�ɻ��p��
		B += '.';
		while (B.length() < A.length())
			B += '0';
	}
	else 								// �p�� + �p�� eg: 123.3457 + 67.891
	{
		dot = std::max(dot1, dot2);
		//�ɻ����
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
		//�ɻ��p��
		while (A.length() < B.length())
			A += '0';

		while (B.length() < A.length())
			B += '0';
	}

	len = A.length();	//A.len == B.len�A�ҥH���Τ��
	int temp;
	if (dot != std::string::npos)
	{
		for (int i = len - 1; i > dot; i--)//�p�Ƴ���
		{
			temp = A[i] + B[i] - 2 * base + carry;
			carry = temp / 10;
			A[i] = temp % 10 + base;
		}
	}
	else
		dot = len;

	for (int i = dot - 1; i >= 0; i--)//��Ƴ���
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

std::string subtraction(std::string A, std::string B)		// ���|�O��-�� (���i�঳�p��)
{
	int dot1 = A.find('.');
	int dot2 = B.find('.');
	int dot;
	int base = '0';
	std::string ans = "";
	char ansSign;

	if (dot1 != std::string::npos && dot2 != std::string::npos)	//�p�� - �p��
	{
		dot = std::max(dot1, dot2);
		//�ɻ����
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
		//�ɻ��p��
		while (A.length() < B.length())
			A += '0';
		while (B.length() < A.length())
			B += '0';
	}
	else if (dot1 == std::string::npos && dot2 == std::string::npos)		// ��� - ���
	{
		while (A.length() < B.length())
			A = '0' + A;
		while (B.length() < A.length())
			B = '0' + B;
	}
	else if (dot1 == std::string::npos)	// ��� - �p��
	{
		dot = (dot2 > A.length()) ? dot2 : A.length();
		// �ɻ����
		while (dot2 < dot)
		{
			B = '0' + B;
			dot2++;
		}
		while (A.length() < dot)
			A = '0' + A;

		//�ɻ��p��
		A += '.';
		while (A.length() < B.length())
			A += '0';
	}
	else								// �p�� - ���
	{
		dot = (dot1 > B.length()) ? dot1 : B.length();
		// �ɻ����
		while (dot1 < dot)
		{
			A = '0' + A;
			dot1++;
		}
		while (B.length() < dot)
			B = '0' + B;

		//�ɻ��p��
		B += '.';
		while (B.length() < A.length())
			B += '0';
	}

	//���AB
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
	//name���Χ�
	digits = rhs.digits;
	dotPlace = rhs.dotPlace;
	sign = rhs.sign;
}

int Parser::compare(std::string B) const
{
	//�^��: -1:<  0:==  1:>

	//if sign is different
	if (B[0] == '-')
	{
		if (sign == 0)
			return 1;
		B.erase(0, 1);//���h�t���A�H�K�᭱�P�_
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
	//�ɻ����
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
	//�ɻ��p��
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
	//if ansDigits = default -> ������0

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
	case '+'://**�[�� & ���t�� ���Ϥ�?
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
	//�M���Ů�
	//infix.erase(std::remove(infix.begin(), infix.end(), ' '), infix.end());//remove space ' '
	for (int i = 0; i < s.length(); i++)
		if (s[i] != ' ')
			infix += s[i];

	//-------------------------------------------
	// ���Ԧ� -> ��Ǧ� <Infix to Postfix>
	//-------------------------------------------
	Parser nowVar;//an number to save into stack
	nowVar.sign = 0;
	std::vector<Parser> postfix, stack;
	Parser now;//an operation to save into stack
	now.sign = 0;
	now.type = 2;
	bool isNegative = false;	//�ϧO�t��&���	
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

		case')': //�@�������ӬA���]��
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
			if (priority > 0)	//���O�Ʀr
			{
				if (infix[i] == '-' && (i == 0 || getPriority(infix[i - 1]) != 0)) //�e�@�Ӥ��O�Ʀr or �o�b���l�̫e���A�o�N�O�t��
				{
					if (i > 2 && infix[i - 1] != '(' && infix[i - 2] != '(') {
						break;
					}

					if (isNegative)
						isNegative = 0;
					else
						isNegative = 1;
					break;	//���t�����|�bstack�̭��A�P�_stack�ɤ~���|�V�c�t��&�
				}

				while (stack.size() > 0)
				{
					if (stack.back().digits == "(")
						break;

					if (priority > getPriority(stack.back().digits[0]))
						break;

					if (priority == 4)//�����O�ѥk�ܥ��B��(2^3^4 == 2^(3^4))
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
				if (i + 1 < infix.length() && getPriority(infix[i + 1]) == 0) //�U�@��]�O�P�ӼƦr or �^��
					nowVar.digits.push_back(infix[i]);

				else { //�U�@�椣�O�Ʀr or �^��
					nowVar.digits.push_back(infix[i]);
					if (nowVar.digits[0] > '9' || nowVar.digits[0] < '0')//�O�@���ܼƦW�� or Power
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
						if (nowVar.dotPlace == string::npos) //�O���
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


	//cout << "\n-------------------- [�B��i��] --------------------\n";
	//-------------------------------------------
	// ���Ǧ� <Evaluate Postfix>
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
			//�B��
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
						cout << "Illegal : �����B�⶷���Υ����\n�Э��s��J : ";
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

	if (stack.back().getDigits().find('.') == string::npos) //�O���
		stack.back().setType(0);
	else
		stack.back().setType(1);

	stack.back().checkType();
	return stack.back();
}

