/*
Параллельные вычисления
Напишите программу для расчёта суммы двух векторов.
Распараллельте эту программу на 2, 4, 8 и 16 потоков.
Определите, какое количество потоков даёт самый быстрый результат.
Сравните результаты выполнения для массивов из 1 000, 10 000, 100 000 и 1 000 000 элементов.
Время выполнения для каждого варианта сведите в таблицу и выведите её в консоль.
Первый запущенный поток должен вывести на экран доступное количество аппаратных ядер.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <Windows.h>
#include <ctime>
#include <exception>

#define ARRAY_SIZE 4

void fillVector(std::vector<int>& v, size_t vectorSize);
std::vector<int> createEmptyVector(size_t vectorSize);
void sumVector(std::vector<int>& v1, std::vector<int>& v2, std::vector<int>& result, size_t startIndex, size_t finishIndex);

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));   // Включаем рандом
    // Включаем русские символы
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    size_t vectorSize[]{ 1000, 10'000, 100'000, 1'000'000 };    // Размеры векторов

    try
    {
        // Создание векторов
        std::unique_ptr<std::vector<int>[]> a{ std::make_unique<std::vector<int>[]>(ARRAY_SIZE) };
        for (size_t i = 0; i < ARRAY_SIZE; ++i)
        {
            fillVector(a[i], vectorSize[i]);    // Заполнение векторов случайными значениями
        }

        std::unique_ptr<std::vector<int>[]> b{ std::make_unique<std::vector<int>[]>(ARRAY_SIZE) };
        for (size_t i = 0; i < ARRAY_SIZE; ++i)
        {
            fillVector(b[i], vectorSize[i]);
        }

        // Создание пустого вектора для хранения результата
        std::unique_ptr<std::vector<int>[]> c{ std::make_unique<std::vector<int>[]>(ARRAY_SIZE) };
        for (size_t i = 0; i < ARRAY_SIZE; ++i)
        {
            c[i] = createEmptyVector(vectorSize[i]);
        }

        // Количество аппаратных ядер
        std::cout << "Количество аппаратных ядер - " << std::thread::hardware_concurrency() << '\n' << std::endl;

        // Начало таблицы
        for (size_t i{}; i < ARRAY_SIZE; ++i)
        {
            std::cout << "\t\t\t" << vectorSize[i];
        }
        std::cout << std::endl;

        // Перебираем количество потоков
        size_t count{};
        while (count < 16)
        {
            if (count != 0)
            {
                count *= 2;
            }
            else
            {
                ++count;
            }
            std::cout << '\n' << count << " потоков\t";

            // Перебираем векторы с разным количеством элементов
            for (size_t i{}; i < ARRAY_SIZE; ++i)
            {
                std::chrono::steady_clock::time_point startTime{ std::chrono::steady_clock::now() };
                std::unique_ptr<std::thread[]> threads{ std::make_unique < std::thread[]>(count) };
                for (size_t tr{}; tr < count; ++tr)
                {
                    size_t startIndex{ tr * c[i].size() / count };
                    size_t finishIndex{ startIndex + (c[i].size() / count) };
                    threads[tr] = std::thread(sumVector, std::ref(a[i]), std::ref(b[i]), std::ref(c[i]), startIndex, finishIndex);
                }
                for (size_t tr{}; tr < count; ++tr)
                {
                    threads[tr].join();
                }
                std::chrono::steady_clock::time_point finishTime{ std::chrono::steady_clock::now() };
                std::chrono::duration<double, std::micro> period{ finishTime - startTime };

                std::cout << "\t" << period.count() << "  mks\t";
            }
            std::cout << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    

    return EXIT_SUCCESS;
}

/// <summary>
/// Заполнение вектора рандомными числами
/// </summary>
/// <param name="v"></param>
/// <param name="vectorSize"></param>
void fillVector(std::vector<int>& v, size_t vectorSize)
{
    for (size_t i{}; i < vectorSize; ++i)
    {
        v.push_back(1 + rand() % 100);
    }
}

/// <summary>
/// Создание пустого вектора
/// </summary>
/// <param name="vectorSize"></param>
/// <returns></returns>
std::vector<int> createEmptyVector(size_t vectorSize)
{
    return std::vector<int>(vectorSize);
}

// Суммирование векторов
void sumVector(std::vector<int>& v1, std::vector<int>& v2, std::vector<int>& result, size_t startIndex, size_t finishIndex)
{
    size_t vectorSize{ v1.size() };
    if (v2.size() != vectorSize)
    {
        throw new std::exception("Wrong vector size!");
    }

    for (size_t i{startIndex}; i < finishIndex; ++i)
    {
        result.at(i) = v1.at(i) + v2.at(i);
    }
}
