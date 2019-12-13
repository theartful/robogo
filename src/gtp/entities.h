#ifndef ENTITIES_H_
#define ENTITIES_H_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
using std::invalid_argument;
using std::out_of_range;
using std::string;
using std::to_string;
using std::vector;
using namespace std;

struct Color;
struct Vertex;
struct Move;
template <typename T>
struct List;
template <typename U>
struct MultiLineList;
template <typename Z, typename H>
struct Alternative;

struct Boolean
{
	string boolean;
	Boolean()
	{
	}

	Boolean(bool value)
	{
		this->boolean = (value) ? "true" : "false";
	}

	Boolean(string value)
	{
		if (!(value == "true" || value == "false"))
			throw invalid_argument("invalid boolean value");

		this->boolean = value;
	}

	string val()
	{
		return boolean;
	}
};

struct Color
{
	string color;
	Color()
	{
	}

	Color(string value)
	{
		if (!(value == "w" || value == "b" || value == "white" ||
		      value == "black"))
			throw invalid_argument("invalid color value");

		this->color = (value == "white" || value == "w") ? "w" : "b";
	}

	string val()
	{
		return color;
	}
};

struct Vertex
{
	string vertex;
	Vertex()
	{
	}

	Vertex(string value)
	{
		if (!(value == "pass"))
		{
			if (value.length() == 2)
			{
				char col = value[1];
				value[1] = '0';
				value.push_back(col);
			}

			if (value.length() > 3)
				throw invalid_argument("invalid vertex value");

			if (!(value[1] >= 48 && value[1] <= 57) &&
			    (value[2] >= 49 && value[2] <= 57))
				throw out_of_range("invalid vertex value");

			if (value[0] == 'i' || value[0] == 'I')
				throw invalid_argument("invalid vertex value");

			int row = (value[1] - 48) * 10 + (value[2] - 48);
			if (row <= 0 || row > 25)
				throw out_of_range("invalid vertex value: protocol doesn't "
				                   "support boards larger than 25x25");

			int column = 0;
			if (value[0] >= 65 && value[0] <= 90)
				column = (value[0] < 73) ? value[0] - 64 : value[0] - 65;
			else if (value[0] >= 97 && value[0] <= 122)
				column = (value[0] < 105) ? value[0] - 96 : value[0] - 97;

			if (column <= 0 || column > 25)
				throw out_of_range("invalid vertex value: protocol doesn't "
				                   "support boards larger than 25x25");
		}

		this->vertex = value;
	}

	Vertex(uint32_t row, uint32_t column)
	{
		if (row == 0 || row > 25 || column == 0 || column > 25)
			throw out_of_range("invalid vertex value: protocol doesn't support "
			                   "boards larger than 25x25");

		char column_letter = (column < 9) ? column + 96 : column + 97;
		string row_str = (to_string(row).length() == 1) ? "0" + to_string(row)
		                                                : to_string(row);

		this->vertex = column_letter + row_str;
	}

	static void indices(const Vertex& vertex, uint32_t& row, uint32_t& column)
	{
		std::stringstream row_stream;
		row_stream << vertex.vertex.substr(1);
		row_stream >> row;

		std::stringstream col_stream;
		int c = vertex.vertex[0];
		col_stream << to_string(c);
		uint32_t col;
		col_stream >> col;

		if (col >= 65 && col <= 90)
			column = (col < 73) ? col - 64 : col - 65;
		else if (col >= 97 && col <= 122)
			column = (col < 105) ? col - 96 : col - 97;
	}

	string val()
	{
		return vertex;
	}
};

struct Move
{
	string move;
	Color color;
	Vertex vertex;
	Move()
	{
	}

	Move(std::string move_str)
	{
		try
		{
			std::istringstream stream(move_str);
			string color_str;
			string vertex_str;
			stream >> color_str;
			stream >> vertex_str;
			color = Color(color_str);
			vertex = Vertex(vertex_str);
		}
		catch (...)
		{
			throw std::invalid_argument("invalid color and vertex value");
		}

		this->move = color.val() + " " + vertex.val();
	}

	Move(Color c, Vertex v)
	{
		color = c;
		vertex = v;
		this->move = color.val() + " " + vertex.val();
	}

	string val()
	{
		return move;
	}
};

template <>
struct List<uint32_t>
{
	vector<uint32_t> items;

	List()
	{
	}

	List(vector<uint32_t> values)
	{
		appendAll(values);
	}

	void append(uint32_t item)
	{
		items.push_back(item);
	}

	void appendAll(vector<uint32_t> values)
	{
		for (vector<uint32_t>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	Boolean includes(uint32_t item)
	{
		for (vector<uint32_t>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (item == *it)
				return Boolean(true);
		}

		return Boolean(false);
	}

	string val()
	{
		string result = "";
		for (vector<uint32_t>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (result == "")
				result = to_string(*it);
			else
				result += " " + to_string(*it);
		}

		return result;
	}
};

template <>
struct List<float>
{
	vector<float> items;

	List()
	{
	}

	List(vector<float> values)
	{
		appendAll(values);
	}

	void append(float item)
	{
		items.push_back(item);
	}

	void appendAll(vector<float> values)
	{
		for (vector<float>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	Boolean includes(float item)
	{
		for (vector<float>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (item == *it)
				return Boolean(true);
		}

		return Boolean(false);
	}

	string val()
	{
		string result = "";
		for (vector<float>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (result == "")
				result = to_string(*it);
			else
				result += " " + to_string(*it);
		}

		return result;
	}
};

template <>
struct List<string>
{
	vector<string> items;
	List()
	{
	}

	List(const string arr[], uint32_t size)
	{
		for (uint32_t i = 0; i < size; i++)
			items.push_back(arr[i]);
	}

	List(vector<string> values)
	{
		appendAll(values);
	}

	void append(string item)
	{
		items.push_back(item);
	}

	void appendAll(vector<string> values)
	{
		for (vector<string>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	Boolean includes(string item)
	{
		for (vector<string>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (item == *it)
				return Boolean(true);
		}

		return Boolean(false);
	}

	string val()
	{
		string result = "";
		for (vector<string>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (result == "")
				result = *it;
			else
				result += " " + *it;
		}

		return result;
	}
};

template <typename T>
struct List
{
	vector<T> items;
	List()
	{
	}

	List(vector<T> values)
	{
		appendAll(values);
	}

	void append(T item)
	{
		items.push_back(item);
	}

	void appendAll(vector<T> values)
	{
		for (int i = 0; i < values.size(); i++)
		{
			this->items.push_back(values[i]);
		}
	}

	bool includes(T item)
	{
		return (
		    find(this->items.begin(), this->items.end(), item) !=
		    this->items.end());
	}

	string val()
	{
		string result = "";
		for (typename vector<T>::iterator it = items.begin(); it != items.end();
		     ++it)
		{
			if (result == "")
				result = it->val();
			else
				result += " " + it->val();
		}

		return result;
	}
};

template <>
struct MultiLineList<uint32_t>
{
	vector<uint32_t> items;

	void append(uint32_t item)
	{
		items.push_back(item);
	}

	void appendAll(vector<uint32_t> values)
	{
		for (vector<uint32_t>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	string val()
	{
		string result = "";
		for (vector<uint32_t>::iterator it = items.begin(); it != items.end();
		     ++it)
			result += to_string(*it) + "\n";

		return result;
	}
};

template <>
struct MultiLineList<float>
{
	vector<float> items;

	void append(float item)
	{
		items.push_back(item);
	}

	void appendAll(vector<float> values)
	{
		for (vector<float>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	string val()
	{
		string result = "";
		for (vector<float>::iterator it = items.begin(); it != items.end();
		     ++it)
			result += to_string(*it) + "\n";

		return result;
	}
};

template <>
struct MultiLineList<string>
{
	vector<string> items;
	MultiLineList()
	{
	}

	MultiLineList(const string arr[], uint32_t size)
	{
		for (uint32_t i = 0; i < size; i++)
			items.push_back(arr[i]);
	}

	void append(string item)
	{
		items.push_back(item);
	}

	void appendAll(vector<string> values)
	{
		for (vector<string>::iterator it = values.begin(); it != values.end();
		     ++it)
			items.push_back(*it);
	}

	string val()
	{
		string result = "";
		for (vector<string>::iterator it = items.begin(); it != items.end();
		     ++it)
			result += *it + "\n";

		return result;
	}
};

template <typename U>
struct MultiLineList
{
	vector<U> items;

	void append(U item)
	{
		items.push_back(item);
	}

	void appendAll(vector<U> values)
	{
		for (int i = 0; i < values.size(); i++)
		{
			this->items.push_back(values[i]);
		}
	}

	string val()
	{
		string result = "";
		for (typename vector<U>::iterator it = items.begin(); it != items.end();
		     ++it)
			result += it->val() + "\n";

		return result;
	}
};

template <typename Z, typename H>
struct Alternative
{
	Alternative() = delete;

	Alternative(Z value)
	{
		this->value1 = value;
		this->is_first = true;
	}

	Alternative(H value)
	{
		this->value2 = value;
	}

	string val()
	{
		string v1;
		string v2;

		if (std::is_same<Z, string>::value)
			v1 = value1;
		else if (
		    std::is_same<Z, uint32_t>::value || std::is_same<Z, float>::value)
			v1 = to_string(value1);
		else
			v1 = value1.val();

		if (std::is_same<H, string>::value)
			v2 = value2;
		else if (
		    std::is_same<H, uint32_t>::value || std::is_same<H, float>::value)
			v2 = to_string(value2);
		else
			v2 = value2.val();

		return (is_first) ? v1 : v2;
	}

private:
	Z value1;
	H value2;
	bool is_first = false;
};

#endif
