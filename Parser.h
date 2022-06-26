#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string> //getline
#include <vector>
#include <cmath>
#include<sstream>
#include <qvector.h>
#define SCALE 200
class Parser {
private:
    int type;	//0:Integer	1:Decimal 2:operator
	bool sign;	//0:+		1:-
	std::string name;
	std::string digits;
	int dotPlace;

public:
	QVector<double> x, y;
	static std::vector<Parser> All; //┮Τ跑计
	Parser() { name = ""; };
	Parser(std::string n) { name = n; };
	Parser(bool, std::string, std::string);//type, name, digits

	void setType(int _type) { type = _type; }
	void setSign(bool _sign) { sign = _sign; }
	void setDigits(std::string _digits) { digits = _digits; }
	void setDotPlace(int _dotPlace) { dotPlace = _dotPlace; }
	void setName(std::string _name)
	{
		//睲埃
		int i = 0;
		while (_name[i] == ' ')
			i++;
		_name.erase(0, i);
		i = _name.length() - 1;
		while (_name[i] == ' ')
			i--;
		_name.erase(i + 1, _name.length() - 1);

		name = _name;
	}

	int getType() { return type; }
	bool getSign() { return sign; }
	std::string getDigits() { return digits; }
	int getDotPlace() { return dotPlace; }
	std::string getName() { return name; }

	void checkType()

	{
		dotPlace = digits.find('.');

		if (type == 0)
		{
			if (dotPlace != std::string::npos)
			{
				digits.erase(dotPlace);
				dotPlace = -1;
			}
		}
		else
		{
			if (dotPlace == std::string::npos)
			{
				dotPlace = digits.length();
				digits += '.';
			}

			if (digits.length() - dotPlace - 1 < 100)//计场だぃì100
			{
				//干100计
				for (int i = digits.length() - dotPlace - 1; i < 100; i++)
					digits += '0';
			}
			else
			{
				digits.erase(dotPlace + 101);
			}
		}
	}
	void test(QString);
	friend Parser process(std::string);
	Parser operator+(Parser) const;
	Parser operator-(Parser) const;
	Parser operator*(Parser) const;
	Parser operator/(Parser) const;
	Parser operator^(Parser) const;
	void operator=(const Parser&);
	bool operator==(std::string zero);
	int compare(std::string) const; //肚: -1:<  0:==  1:>
	friend std::ostream& operator<<(std::ostream&, const Parser&);
};


#endif // PARSER_H

//y=x+5

