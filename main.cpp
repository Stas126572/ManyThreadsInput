// ConsoleApplication129.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

// Type your code here, or load an example.
#include <stdio.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <mutex>

template<size_t ss>
class ManyThreadsTyper
{
public:

	unsigned int column;
	bool is_any_threads_write;
	bool need_continue;
	bool is_buf_flush;
	char buffer[ss + 1];
	size_t start_write_pos;
	std::condition_variable wait_for, wait_for_full_buf, wait_for_in;
	std::thread control, BufOverFlow;
	std::mutex mutex_to_wait, mutex_to_wait2;
	size_t size_of_buffer;
	bool can_write;

	void flush_buffer()
	{
		can_write = false;
		is_buf_flush = true;
		buffer[__min(ss, start_write_pos)] = 0;
		fputs(buffer, stdout);
	}
	~ManyThreadsTyper()
	{
		need_continue = false;
		if (column == 0)
		{
			wait_for.notify_one();
		}
		control.join();
		wait_for_full_buf.notify_one();
		BufOverFlow.join();
	}

	ManyThreadsTyper()
	{
		can_write = false;
		need_continue = true;
		column = 0;
		is_buf_flush = false;
		std::unique_lock<std::mutex> _need_continue(mutex_to_wait);
		set_start_parametrs(_need_continue);
		control = std::thread(&ManyThreadsTyper::Controller, this);
		BufOverFlow = std::thread(&ManyThreadsTyper::BufOverflow, this);
	}



	void set_start_parametrs(std::unique_lock<std::mutex>& ul)
	{
		is_any_threads_write = false;
		start_write_pos = 0;
		ul.unlock();
	}

	void BufOverflow()
	{
		while (need_continue)
		{
			std::unique_lock<std::mutex> wait_unique_lock(mutex_to_wait);
			while ((start_write_pos < ss) && need_continue)
			{
				wait_for_full_buf.wait(wait_unique_lock);
			}

			if (!need_continue)
			{
				break;
			}
			
			if (is_buf_flush == false)
			flush_buffer();
			set_start_parametrs(wait_unique_lock);
			wait_for_in.notify_all();
		}

	}

	void Controller()
	{
		while (need_continue)
		{
			std::unique_lock<std::mutex> _need_continue(mutex_to_wait2);
			can_write = true;
			std::cerr << "00";
			wait_for_in.notify_all();
			while (!is_any_threads_write && need_continue)
			{
				wait_for.wait(_need_continue);
			};
			while (column != 0)
			{
				wait_for.wait(_need_continue);
			}

			if (is_buf_flush == false)
			flush_buffer();
			set_start_parametrs(_need_continue);
		}
		
	}


	void Print(const char* string, size_t size)
	{
		size_t start_pos, end_pos;


		bool IsBufOverflow = false;
		std::unique_lock<std::mutex> _can_write(mutex_to_wait2);
		std::cerr << "0    ";
		while (!can_write)
		{
			wait_for_in.wait(_can_write);
			std::cerr << can_write;
		}
		std::cerr << "1    ";
		
		std::unique_lock<std::mutex> need_to_write(mutex_to_wait);

		start_write_pos += size;
		is_any_threads_write = true;
		column++;
        is_buf_flush = false;
		_can_write.unlock();

		std::cerr << "3    ";
		if (start_write_pos > ss)
		{
			wait_for_full_buf.notify_one();
			while (start_write_pos != 0)
			{
				wait_for_in.wait(need_to_write);
			}
			std::cerr << "2    ";
			IsBufOverflow = true;
		}
		if (IsBufOverflow)
		{
			start_pos = start_write_pos;
			end_pos = start_write_pos + size;
			start_write_pos = end_pos;
		}
		else
		{
			end_pos = start_write_pos;
			start_pos = start_write_pos - size;
		}


		need_to_write.unlock();
		wait_for.notify_one();


		for (size_t i = 0; i < size; i++)
		{
			buffer[i+start_pos] = string[i];
		}

		_can_write.lock();
		column--;
		_can_write.unlock();

	}

	void Print(std::string& s)
	{
		Print((const char*)s.c_str(), s.length());
	}

};

template<size_t s>
void Thread1(ManyThreadsTyper<s>& mt)
{
	std::string s1 = " 11";
	std::string s2 = " 12";

	mt.Print(s1);
	mt.Print(s2);
}

template<size_t s>
void Thread2(ManyThreadsTyper<s>& mt)
{
	std::string s = " 21";
	mt.Print(s);
}

template<size_t s>
void Thread3(ManyThreadsTyper<s>& mt)
{
	std::string s = " 31";
	std::string s2 = " 32";
	mt.Print(s);
	mt.Print(s2);
}



int main()
{
	ManyThreadsTyper<3> mt;
	std::thread t1(Thread1<3>, std::ref(mt));
	std::thread t2(Thread2<3>, std::ref(mt));
	std::thread t3(Thread3<3>, std::ref(mt));
	t2.join();
	t1.join();
	t3.join();
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
