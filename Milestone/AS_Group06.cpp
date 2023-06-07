#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <occi.h>

using namespace oracle::occi;
using namespace std;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char extension[10];
	char email[100];
	char officeCode[10];
	int reportsTo[100];
	char jobTitle[50];
};

int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp) {
	try {
		Statement* stmt = conn->createStatement("SELECT * FROM employees "
			"WHERE employeeNumber = :empNum");
		stmt->setInt(1, employeeNumber);
		ResultSet* rs = stmt->executeQuery();
		if (rs->next()) {
			emp->employeeNumber = rs->getInt(1);
			strcpy(emp->lastName, rs->getString(2).c_str());
			strcpy(emp->firstName, rs->getString(3).c_str());
			strcpy(emp->extension, rs->getString(4).c_str());
			strcpy(emp->email, rs->getString(5).c_str());
			strcpy(emp->officeCode, rs->getString(6).c_str());
			emp->reportsTo[0] = rs->getInt(7);
			strcpy(emp->jobTitle, rs->getString(8).c_str());
			return 1;
		}
		else {
			return 0;
		}
	}
	catch (SQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		return 0;
	}
}
void displayEmployee(Connection* conn, struct Employee emp) {
	cout << endl;
	cout << "-------------- Employee Information -------------" << endl;
	cout << "Employee Number: " << emp.employeeNumber << endl;
	cout << "Last Name: " << emp.lastName << endl;
	cout << "First Name: " << emp.firstName << endl;
	cout << "Extension: " << emp.extension << endl;
	cout << "Email: " << emp.email << endl;
	cout << "Office Code: " << emp.officeCode << endl;
	cout << "Manager ID: " << emp.reportsTo[0] << endl;
	cout << "Job Title: " << emp.jobTitle << endl;

}
void displayAllEmployees(Connection* conn) {
	try {
		Statement* stmt = conn->createStatement();
		ResultSet* rs = stmt->executeQuery("SELECT E.EMPLOYEENUMBER, E.FIRSTNAME ||' '|| E.LASTNAME,"
			"				E.EMAIL, A.PHONE, E.EXTENSION, M.FIRSTNAME||' '|| M.LASTNAME"
			"				FROM EMPLOYEES E LEFT JOIN EMPLOYEES M ON E.REPORTSTO = M.EMPLOYEENUMBER"
			"				LEFT JOIN OFFICES A ON E.OFFICECODE = A.OFFICECODE"
			"				ORDER BY E.EMPLOYEENUMBER");
		cout << "------   ---------------   ------------------------------------------   ----------------   ---------   -----------------" << endl;
		cout << "ID       EMPLOYEE NAME     Email                                        Phone              Extension   Manager Name" << endl;
		cout << "------   ---------------   ------------------------------------------   ----------------   ---------   -----------------" << endl;
		while (rs->next()) {
			int ID = rs->getInt(1);
			string ENAME = rs->getString(2);
			string EEMAIL = rs->getString(3);
			string EPHONE = rs->getString(4);
			string EEXTENSION = rs->getString(5);
			string MNAME = rs->getString(6);

			cout.setf(ios::left);
			cout.width(9);
			cout << ID;
			cout.width(18);
			cout << ENAME;
			cout.width(45);
			cout << EEMAIL;
			cout.width(19);
			cout << EPHONE;
			cout.width(12);
			cout << EEXTENSION;
			cout.width(17);
			cout << MNAME;
			cout << endl;
		}
		cout << endl;
	}
	catch (SQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}
}
void updateEmployee(Connection* conn, int employeeNumber) {
	struct Employee emp = {};
	Statement* stmt = nullptr;
	int selection = findEmployee(conn, employeeNumber, &emp);
	if (selection) {
		cout << "Last Name: " << emp.lastName << endl;
		cout << "First Name: " << emp.firstName << endl;
		cout << "Extension: ";
		cin >> emp.extension;

		stmt = conn->createStatement("UPDATE employees SET extension = :1 WHERE employeenumber = :2");
		stmt->setString(1, emp.extension);
		stmt->setInt(2, emp.employeeNumber);
		stmt->executeUpdate();
		conn->commit();
		conn->terminateStatement(stmt);

		cout << "The employee's extension is updated successfully." << endl;
	}
	else
	{
		cout << "The employee with ID " << employeeNumber << " does not exist." << endl;
	}
}

void addEmployee(Connection* conn, struct Employee emp)
{
	Statement* stmt = nullptr;
	ResultSet* rst = nullptr;
	int valAgain = findEmployee(conn, emp.employeeNumber, &emp);
	if (valAgain != 1)
	{
		try {
			stmt = conn->createStatement();
			rst = stmt->executeQuery("INSERT INTO dbs211_employees VALUES ( " + to_string(emp.employeeNumber) +
				", '" + emp.lastName + "', '" + emp.firstName +
				"', '" + emp.extension + "', '" + emp.email + "', " +
				*emp.officeCode + ", " + to_string(*emp.reportsTo) + ", '" + emp.jobTitle + "' )");
			conn->commit();
			conn->terminateStatement(stmt);
			cout << "\n" << "The new employee is added successfully." << "\n" << "\n";
		}
		catch (SQLException& sqlExcp) {
			cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		}
	}
	else { cout << "An employee with the same employee number exists." << "\n" << "\n"; }
}

void removeEmployee(Connection* conn, int employeeNumber) {
	Employee emp{};
	Statement* stmt = nullptr; ResultSet* rst = nullptr;
	int valAgain = findEmployee(conn, employeeNumber, &emp);
	if (valAgain == 1) {
		try {
			stmt = conn->createStatement();
			rst = stmt->executeQuery
			("DELETE FROM dbs211_employees WHERE employeenumber = " + to_string(employeeNumber));
			conn->commit();
			conn->terminateStatement(stmt);
			cout << "The employee with ID " << employeeNumber << " is deletedsuccessfully.";
			cout << "\n";
			cout << "\n";
		}
		catch (SQLException& sqlExcp)
		{
			cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
		}
	}
	else {
		cout << "The employee with ID " << employeeNumber << " does not exist";
		cout << "\n";
		cout << "\n";
	}
}


int main() {
	Environment* env = Environment::createEnvironment(Environment::DEFAULT);
	Connection* conn = env->createConnection("dbs211_231nhh16", "19662431",
		"myoracle12c.senecacollege.ca:1521/oracle12c");
	int a;
	struct Employee emp;
	do {
		cout << "******************** HR Menu ********************" << endl;
		cout << "1) Find Employee" << endl;
		cout << "2) Employees Report" << endl;
		cout << "3) Add Employee" << endl;
		cout << "4) Update Employee" << endl;
		cout << "5) Remove Employee" << endl;
		cout << "0) Exit" << endl;
		cout << "Enter an option (0-5): ";
		cin >> a;
		if (a > 5 || a < 0) {
			while (a > 5 || a < 0) {
				cout << "Invalid Entry" << endl;
				cout << "Enter an option (0-5): ";
				cin >> a;
			}
		}
		switch (a) {
		case 1:
			int employeeNum;
			cout << "Enter the employee number: ";
			cin >> employeeNum;
			if (findEmployee(conn, employeeNum, &emp) == 1) {
				displayEmployee(conn, emp);
			}
			else {
				cout << "Employee " << employeeNum << " does not exist." << endl;
			}
			cout << endl;
			break;
		case 2:
			displayAllEmployees(conn);
			break;
		case 3:
			addEmployee(conn, emp);
			break;
		case 4:
			cout << "Enter the employee number: ";
			cin >> employeeNum;
			updateEmployee(conn, employeeNum);
			break;
		case 5:
			cin >> employeeNum;
			removeEmployee(conn, employeeNum);
				break;
		}

	} while (a != 0);

	return 0;
}
