#include <string>

using namespace std;

class Variable {
private:
	double power;
	string name;
	string label;

public:
	Variable(const double & power, string name);
	double getPower() const;
	string getName() const;
	string getLabel() const;
	void createLabel();
	void setPower(const double & newPower);
};