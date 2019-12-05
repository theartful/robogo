#ifndef ENTITIES_H_
#define ENTITIES_H_

#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;
using std::to_string;
using std::invalid_argument;
using std::out_of_range;

struct Color;
struct Vertex;
struct Move;
template<typename T> struct List;
template<typename U> struct MultiLineList;
template <typename Z, typename H> struct Alternative;


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

    operator string ()
    {
        return this->boolean;
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
        if (!(value == "w" || value == "b" || value == "white" || value == "black"))
            throw invalid_argument("invalid color value");

        this->color = value;
    }

    operator string ()
    {
        return this->color;
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
            if (value.length() > 3)
                throw invalid_argument("invalid vertex value");

            if (!(value[1] >= 48 && value[1] <= 57) && (value[2] >= 49 && value[2] <= 57))
                throw out_of_range("invalid vertex value");

            if (value[0] == 'i' || value[0] == 'I')
                throw invalid_argument("invalid vertex value");

            uint32_t row = (value[1] - 48) * 10 + (value[2] - 48);
            if (row > 25)
                throw out_of_range("invalid vertex value: protocol doesn't support boards larger than 25x25");

            uint32_t column = 0;
            if (value[0] >= 65 && value[0] <= 90)
                column = (value[0] < 73) ? value[0] - 64 : value[0] - 65;
            else if (value[0] >= 97 && value[0] <= 122)
                column = (value[0] < 105) ? value[0] - 96 : value[0] - 97;

            if (column > 25)
                throw out_of_range("invalid vertex value: protocol doesn't support boards larger than 25x25");
        }

        this->vertex = value;
    }

    Vertex(uint32_t row, uint32_t column)
    {
        if (row > 25 || column > 25)
            throw out_of_range("invalid vertex value: protocol doesn't support boards larger than 25x25");

        char column = (column < 105) ? column + 96 : column + 97;
        string row_str = (to_string(row).length() == 1) ? "0" + to_string(row) : to_string(row);
        this->vertex = to_string(column) + row_str;
    }

    operator string()
    {
        return this->vertex;
    }
};

struct Move
{
    string move;
    Move()
    {
    }

    Move(Color color, Vertex vertex)
    {
        this->move = (string)color + (string)vertex;
    }

    operator string ()
    {
        return this->move;
    }
};

template<> struct List<uint32_t>
{
    vector<uint32_t> items;

    void append(uint32_t item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(vector<uint32_t>::iterator it = items.begin(); it != items.end(); ++it) {
            if (result == "")
                result = to_string(*it);
            else
                result += " " + to_string(*it);
        }

        return result;
    }
};

template<> struct List<float>
{
    vector<float> items;

    void append(float item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(vector<float>::iterator it = items.begin(); it != items.end(); ++it)
        {
            if (result == "")
                result = to_string(*it);
            else
                result += " " + to_string(*it);
        }

        return result;
    }
};

template<typename T> struct List
{
    vector<T> items;

    void append(T item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(typename vector<T>::iterator it = items.begin(); it != items.end(); ++it)
        {
            if (result == "")
                result = (string)*it;
            else
                result += " " + (string)*it;
        }

        return result;
    }
};

template<> struct MultiLineList<uint32_t>
{
    vector<uint32_t> items;

    void append(uint32_t item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(vector<uint32_t>::iterator it = items.begin(); it != items.end(); ++it)
            result += to_string(*it) + "\n";

        return result;
    }
};

template<> struct MultiLineList<float>
{
    vector<float> items;

    void append(float item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(vector<float>::iterator it = items.begin(); it != items.end(); ++it)
            result += to_string(*it) + "\n";

        return result;
    }
};

template<typename U> struct MultiLineList
{
    vector<U> items;

    void append(U item)
    {
        items.push_back(item);
    }

    operator string ()
    {
        string result = "";
        for(typename vector<U>::iterator it = items.begin(); it != items.end(); ++it)
            result += (string)*it + "\n";

        return result;
    }
};

template <typename Z, typename H> struct Alternative
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

    operator string()
    {
        return (is_first) ? (string)this->value1 : (string)this->value2;
    }

    private:
        Z value1;
        H value2;
        bool is_first = false;
};

#endif
