//Author:	Yi-Hao Chen, Hung-Xin Chen
//License:	GPL-2 | GPL-3 [expanded from: GPL (≥ 2)]

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <utility>
#include <algorithm>
using namespace std;
class SampleRow : public pair < string, vector<double> >
{
public:
	SampleRow()
	{

	}
	SampleRow(string name, vector<double> data)
	{
		first = name;
		second = data;
	}
	SampleRow(string name, double data)
	{
		first = name;
		second.resize(1);
		second[0] = data;
	}
	double average() const
	{
		double val = 0.0;
		auto len = second.size();
		for (size_t i = 0; i < len; i++)
			val += second[i];
		return val / len;
	}
	const string& getName() const
	{
		return first;
	}
	SampleRow& setName(const string& name)
	{
		first = name;
		return *this;
	}
	double& operator[](const int &i)
	{
		return second[i];
	}
	double& operator[](const int &&i)
	{
		return second[i];
	}
	void clear()
	{
		first.clear();
		second.clear();
	}
};
class SampleTable : public vector<SampleRow>
{
public:
	SampleTable getAvgTable() const
	{
		SampleTable out;
		vector<double> a(1);
		for (size_t i = 0; i < size(); i++)
			out.push_back(SampleRow((*this)[i].getName(), (*this)[i].average()));
		return out;
	}
	//divide by avg
	SampleTable operator/(const SampleTable &b)
	{
		SampleTable avg1 = getAvgTable();
		SampleTable avg2 = b.getAvgTable();
		SampleTable out(avg1);
		size_t len;
		out.resize(len = avg1.size());
		for (size_t i = 0; i < len; i++)
		{
			if ((avg1[i][0] < 1 && avg2[i][0] < 1) || avg1[i][0] == 0.0 || avg2[i][0] == 0.0)
				continue;
			out[i][0] = (avg1[i][0] / avg2[i][0]);
		}
		return out;
	}
	pair<SampleTable, SampleTable> getZeroTable(const SampleTable &b)
	{
		pair<SampleTable, SampleTable> out;
		SampleTable avg1 = getAvgTable();
		SampleTable avg2 = b.getAvgTable();
		size_t len = avg1.size();
		for (size_t i = 0; i < len; i++)
		{
			if (avg1[i][0] == 0.0 || avg2[i][0] == 0.0)
			{
				out.first.push_back(avg1[i]);
				out.second.push_back(avg2[i]);
			}
		}
		return out;
	}
	//sort function is only for average (that is the size of second is 1)
	SampleTable& sortToMin()
	{
		sort(begin(), end(),
			[](SampleRow &A, SampleRow &B) -> bool
		{
			return A.second[0] > B.second[0];
		}
		);
		return *this;
	}
	SampleTable& sortToMax()
	{
		sort(begin(), end(),
			[](SampleRow &A, SampleRow &B) -> bool
		{
			return A.second[0] < B.second[0];
		}
		);
		return *this;
	}
};
class CSampleReader
{
public:
	CSampleReader()
	{

	}
	CSampleReader(const char* path)
	{
		if (!open(path))
			cout << "Open file failed!\n";
		else
			read();
	}
	bool open(const char* path)
	{
		inFile.close();
		inFile.open(path);
		return inFile.is_open();
	}
	SampleTable read(const char *path)
	{
		dataTable.clear();
		if (!open(path))
			cout << "Open file failed!\n";
		else
			read();
		return dataTable;
	}
	SampleTable cloneData()
	{
		return dataTable;
	}
private:
	bool readRow(SampleRow &data)
	{
		string buf, div;
		stringstream ss;
		auto str2double = [](const string &str)
		{
			stringstream ss;
			double val;
			ss << str;
			ss >> val;
			return val;
		};
		getline(inFile, buf);
		if (!inFile.is_open() || inFile.eof())
			return false;
		ss << buf;
		getline(ss, div, ',');
		data.first = div;
		while (getline(ss, div, ','))
			data.second.push_back(str2double(div));
		return true;
	}
	void read()
	{
		SampleRow data;
		while (readRow(data))
		{
			dataTable.push_back(data);
			data.clear();
		}
	}
	ifstream inFile;
	SampleTable dataTable;
};
int main(void)
{
	CSampleReader reader;
	SampleTable A, B, C;
	streambuf *bk;
	ofstream fileOutA("outA.txt");
	ofstream fileOutB("outB.txt");
	ofstream fileOutB_A("outB_A.txt");
	ofstream fileOutSort("outB_A_Sort.txt");
	bk = cout.rdbuf();


	A = reader.read("a.csv");
	B = reader.read("b.csv");
	C = B / A;

	cout.rdbuf(fileOutA.rdbuf());
	cout << "Data A.csv Avg \n";
	for (size_t i = 0; i < A.size(); i++)
	{
		cout << A[i].getName() << ":" << A[i].average() << endl;
	}

	cout.rdbuf(fileOutB.rdbuf());

	cout << "Data B.csv Avg \n";
	for (size_t i = 0; i < B.size(); i++)
	{
		cout << B[i].getName() << ":" << B[i].average() << endl;
	}


	cout.rdbuf(fileOutB_A.rdbuf());

	cout << "Data B/A average \n";

	for (size_t i = 0; i < C.size(); i++)
	{
		cout << C[i].getName() << " Avg:" << C[i][0] << endl;
	}

	cout.rdbuf(fileOutSort.rdbuf());
	cout << "Data B/A sort\n";

	C.sortToMin();
	for (size_t i = 0; i < C.size(); i++)
	{
		cout << C[i].getName() << " Avg:" << C[i][0] << endl;
	}
	fileOutA.close();
	fileOutB.close();
	fileOutB_A.close();
	fileOutSort.close();
	cout.rdbuf(bk);
	return 0;
}
