// MergerBigSortFile.cpp
// Программа для слияния больших сортированных файлов
// подаём на вход два файла File_1.txt и File_2.txt
// (файлы могут содержать пустые строки)
// на выходе получаем файл MergedFile.txt
// Файлы поместить в папку с программой.


#include <iostream>
#include <string>
#include <fstream>
#include <chrono>


// Названия файлов
const std::string firstFileName = "File_1.txt";
const std::string secondFileName = "File_2.txt";
const std::string outFileName = "MergedFile.txt";

// Указатель из какого файла производить чтение
enum ReadFile
{
    READFILE_BOTH,
    READFILE_FIRST,
    READFILE_SECOND,
    READFILE_UNACCOUNTED
};


// Таймер
class Timer
{
private:
    // Псевдонимы типов для удобства
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t> m_beg;

public:
    Timer() : m_beg(clock_t::now())
    {
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }
};


// Задержка закрытия программы
void delay()
{
    std::cout << "Press any key to close..." << std::endl;

    std::cin.clear();
    std::cin.ignore(32767, '\n');
    std::cin.get();
}


// Функция подсчёта записанных строк в файл out
void counter(Timer& t, int distance = 1)
{
    static long indexWrite = 1;

    if (distance)
    {
        if (indexWrite % 10'000'000 == 0)
        {
            std::cout << indexWrite / 1'000'000 << "kk lines recorded. Time: " << t.elapsed() << std::endl;
        }

        ++indexWrite;
    }
    else
    {
        std::cout << "The " << outFileName << " contains " << indexWrite << " lines." << std::endl;
    }
}

int main()
{
    std::cout << "The start of the merge program." << std::endl;
    Timer t;


    // Открываем первый файл
    std::ifstream firstInFile(firstFileName);

    // Проверяем, что файл открыт
    if (!firstInFile.is_open())
    {
        std::cerr << "ERROR: The first In-file did not open!" << std::endl;

        // Задержка закрытия
        delay();

        return -1;
    }


    // Открываем второй файл
    std::ifstream secondInFile(secondFileName);

    // Проверяем, что второй файл открыт
    if (!secondInFile.is_open())
    {
        std::cerr << "ERROR: The second In-file did not open!" << std::endl;
        firstInFile.close();

        delay();

        return -1;
    }


    // Открываем файл для записи
    std::ofstream outFile(outFileName);

    // Проверяем, что файл записи открыт
    if (!outFile.is_open())
    {
        std::cerr << "ERROR: The out file did not open!" << std::endl;
        firstInFile.close();
        secondInFile.close();

        delay();

        return -1;
    }


    // Записываем строки в файл записи
    std::cout << "Start merging files. Time: " << t.elapsed() << std::endl;

    // READFILE_BOTH - считываем значения из обоих файлов,
    // READFILE_FIRST - только из первого,
    // READFILE_SECOND - только из второго
    ReadFile readTurn = READFILE_BOTH;

    std::string firstFileStr;
    std::string secondFileStr;
    // Работаем пока один из файлов не закончится
    while (!firstInFile.eof() && !secondInFile.eof())
    {
        // Считываем строки из файлов
        if (readTurn == READFILE_FIRST || readTurn == READFILE_BOTH)
        {
            do
            {
                std::getline(firstInFile, firstFileStr);
            } while (firstFileStr == "" && !firstInFile.eof()); // проверка на пустую строку и конец файла
        }

        if (readTurn == READFILE_SECOND || readTurn == READFILE_BOTH)
        {
            do
            {
                std::getline(secondInFile, secondFileStr);
            } while (secondFileStr == "" && !secondInFile.eof());
        }

        // Если файл закончился, а последняя считаная строка пустая
        if (firstFileStr == "" || secondFileStr == "")
        {
            // Указываем, что в памяти есть неучтённая строка
            readTurn = READFILE_UNACCOUNTED;

            break;
        }

        // Сравниваем строки, записываем меньшую в файл
        if (firstFileStr < secondFileStr)
        {
            outFile << firstFileStr << std::endl;
            readTurn = READFILE_FIRST;
        }
        else if (firstFileStr > secondFileStr)
        {
            outFile << secondFileStr << std::endl;
            readTurn = READFILE_SECOND;
        }
        else // если строки равны
        {
            outFile << firstFileStr << std::endl;
            readTurn = READFILE_BOTH;
        }

        counter(t);
    }


    // Дописываем строки оставшегося файла в конец Out.txt
    std::cout << "The program writes the remaining file. Time: " << t.elapsed() << std::endl;

    // Если первый файл на закончился
    if (!firstInFile.eof())
    {
        std::cout << "Adding the first file. Time: " << t.elapsed() << std::endl;

        // В памяти осталась одна неучтённая строка
        if (readTurn != READFILE_BOTH && firstFileStr != "")
        {
            outFile << firstFileStr << std::endl;

            counter(t);
        }

        while (!firstInFile.eof())
        {
            std::getline(firstInFile, firstFileStr);

            if (firstFileStr != "")
            {
                outFile << firstFileStr << std::endl;

                counter(t);
            }
        }
    }

    // Если второй файл на закончился
    if (!secondInFile.eof())
    {
        std::cout << "Adding the second file. Time: " << t.elapsed() << std::endl;

        // В памяти осталась одна неучтённая строка
        if (readTurn != READFILE_BOTH && secondFileStr != "")
        {
            outFile << secondFileStr << std::endl;

            counter(t);
        }

        while (!secondInFile.eof())
        {
            std::getline(secondInFile, secondFileStr);

            if (secondFileStr != "")
            {
                outFile << secondFileStr << std::endl;

                counter(t);
            }
        }
    }


    // Закрываем файлы
    firstInFile.close();
    secondInFile.close();
    outFile.close();

    // Выводим количество строк в Out файле
    counter(t, 0);

    std::cout << "End of the program. Time: " << t.elapsed() << std::endl;


    delay();

    return 0;
}
