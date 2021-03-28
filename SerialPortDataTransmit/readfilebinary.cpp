#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <bitset>
 
int main(int argc, char *argv[])
{
 
        std::string full_path = argv[1];
        
        std::ifstream is;
        is.open(full_path.c_str(), std::ios::in | std::ios::binary);//ios::binary - бинарное открытие,ios::in - операции ввода
        if (!is)
        {
            std::cout << "Файл не найден";
            return 0;
        }
        char buf[512];
        std::string content;
        while (is.read(buf, sizeof(buf)).gcount() > 0)//если количество символов, прочтенных при последней операции неформатированного ввода >0
        {
            content.append(buf, is.gcount());// то добавляем в строку это кол-во символов.
            std::cout << '*';
        }
        std::ofstream fout("copy_test.exe", std::ofstream::binary);// создаём объект класса ofstream для записи и связываем его с файлом 
 
        fout << content;
        fout.close(); // закрываем файл
    
}