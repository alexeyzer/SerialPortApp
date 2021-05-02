#include <string>
#include <bitset>
#include <fstream>
#include <iostream>
#include <ostream>
#include <bitset>
using namespace std;

bitset<7> bits;


char *decoder(char *bytes)
{
	char *result;
	int i =0;
	result = (char*)calloc(1,sizeof(char));
	*result |= ((bytes[0] >> 2) & 1);
	*result |= (((bytes[0] >> 4) & 1) << 1);
	*result |= (((bytes[0] >> 5) & 1) << 2);
	*result |= (((bytes[0] >> 6) & 1) << 3);
	*result |= (((bytes[1] >> 2) & 1) << 4);
	*result |= (((bytes[1] >> 4) & 1) << 5);
	*result |= (((bytes[1] >> 5) & 1) << 6);
	*result |= (((bytes[1] >> 6) & 1) << 7);
	/*cout << "bb";
	while (i < 8)
	{
		cout << ((*result >> i) & 1);
		i++;
	}
	i = 0;
	cout << endl;*/
	*result = (char)*result;
	return (result);
}

int iserror(char *bytes)
{
	int i;

	i = 0;
	while (i < 2)
	{
		if (((bytes[i]) & 1) ^ ((bytes[i] >> 2) & 1) ^ ((bytes[i] >> 4) & 1) ^ ((bytes[i] >> 6) & 1))
			return (1);
		if (((bytes[i] >> 1) & 1) ^ ((bytes[i] >> 2) & 1) ^ ((bytes[i] >> 5) & 1) ^ ((bytes[i] >> 6) & 1))
			return (1);
		if (((bytes[i] >> 3) & 1) ^ ((bytes[i] >> 4) & 1) ^ ((bytes[i] >> 5) & 1) ^ ((bytes[i] >> 6) & 1))
			return (1);
		i++;
	}
	return (0);
}

char *codder(char byte)
{
	int i;
	int j;
	int k;
	char *a;

	i = 0;
	j = 0;
	k = 2;
	a = (char*)calloc(2, sizeof(char));
	/*cout << "aa";
	while (i < 8)
	{
		cout << ((byte >> i) & 1);
		i++;
	}
	i = 0;
	cout << endl;*/
	while (i < 8)
	{
		a[j] = a[j] | (((byte >> i) & 1) << k);
		if (k < 6 && k >= 4) 
			k++;
		else if (k == 2)
			k += 2;
		else if (k == 6)
		{
			k = 2;
			j++;
		}
		i++;
	}
	i = 0;
	while (i < 2)
	{
		a[i] |= ((a[i] >> 2) & 1) ^ ((a[i] >> 4) & 1) ^ ((a[i] >> 6) & 1);
		a[i] |= (((a[i] >> 2) & 1) ^ ((a[i] >> 5) & 1) ^ ((a[i] >> 6) & 1)) << 1;
		a[i] |= (((a[i] >> 4) & 1) ^ ((a[i] >> 5) & 1) ^ ((a[i] >> 6) & 1)) << 3;
		i++;
	}
	/*j =0;
	while (j < 2)
	{
		i = 0;
		while (i < 8)
		{
			cout << (a[j] >> i & 1);
			i++;
		}
		cout << endl;
		j++;
	}
	return ("a");*/
	return (a);
}

void decodeanndwritetofile(char *memoery, int syze)
{
	int i;
	char *temp;
	ofstream w("./Text1.txt", fstream::out);
	i = 0;
	while (i < syze * 2)
	{
		temp = decoder(&(memoery[i]));
		i +=2;
		w.write(temp,1);
		free(temp);
	}
}

int main(int argc, char *argv[])
{
    ifstream f("./Text.txt", ios::binary | ios::in);
	ofstream w("./Text1.txt", ios::binary | fstream::out);
	char *buff = NULL;
	char *temp = NULL;
	char* mem = NULL;
	int now = 0;
	int nowtowrite = 0;
	buff = (char*)calloc(500, sizeof(char));
	int strsize = 0;
	int readed = 0;

	while ((readed = f.read(buff, 500).gcount()) > 0)
	{
		if (mem == NULL)
		{
			strsize = readed;
			mem = (char*)calloc(readed, sizeof(char));
			memcpy(mem, buff, readed);
		}
		else
		{
			temp = mem;
			mem = (char*)calloc(strsize + readed, sizeof(char));
			memcpy(mem, temp, strsize);
			memcpy(mem + strsize, buff, readed);
			free(temp);
			strsize = strsize + readed;
		}
	}
	free(buff);
	//cout << (int)'1'<< "k";
	temp = (char*)calloc((strsize * 2),sizeof(char));
	while (now < strsize)
	{
		buff = codder(mem[now]);
		memcpy(temp + nowtowrite, buff, 2);
		nowtowrite +=2;
		now++;
		free(buff);
	}
	free(mem);
	mem = temp;
	temp = NULL;
	now = 0;
	while (now < strsize * 2)
	{
		if (iserror(&(mem[now])) > 0)
			cout << "error" << endl;
		now +=2;
	}
	decodeanndwritetofile(mem, strsize);
}