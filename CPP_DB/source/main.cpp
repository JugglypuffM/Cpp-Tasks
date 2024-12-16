#include "main.h"
// Библиотеки для работы с временем
#include <iomanip>
#include <chrono>
// Пути до файлов
#define LOG_PATH_TABLE_HASH "logs/table_hash.log"
#define LOG_PATH_MAIN "logs/main.log"
#define LOG_PATH_TABLES "logs/tables.log"
// Константы для хэш-функции
#define C 11
#define REMAINDER 1000000

// Глобальные переменные

// Потоки для записи логов
ofstream out_table_hash, out_main, out_table;
// Общий поток для считывания файлов
ifstream in;
// Словарь таблиц
map <string, Table*> tables;
// Словарь ключевых атрибутов таблиц
map <string, string[2]> key_attributes;
// Ключи словаря
string* table_names; int amount_tables = 0;
// Константа пути к таблицам (не через define, т. к. используется в других файлах)
const string PATH_TABLES = "tables/";
// Словарь размера типов данных
map<string, int> size_of_types;
// Константа для запросов поиска к таблицам, обозначает что атрибут под этим индексом может быть любым.
const string FLAG = "%&";
// Название операции
string input;


// Функция для записи сообщения в файл с логами. Одна для всех лог-файлов.
// Да, логи ведутся, ведь у любой БД должны быть логи, не так ли?
void write_to_log(string out, string message, int indent);

// Функция для посчёта количества вхождений символа в строку
// (подключать целый algorithm ради одного count - такое)
int count(string str, char ch) {
	int n = (int)str.length(), res = 0;
	for (int i = 0; i < n; i++) if (str[i] == ch) res++;
	return res;
}

// Функция, которая инициализирует потоки для записи логов
void init_logs_streams();

// Функция, которая закрывает потоки для записи логов
void close_logs_streams();

// Функция, которая считывает header.txt и заполняет словарь tables.
void parse_header();

// Функция, которая вызывает физическое удаление у всех таблиц
void clear_deleted() {
	for (int i = 0; i < amount_tables; i++)
		tables[table_names[i]]->physically_clear_entries();
}

// Функция, разбивающая строку из таблицы на массив и удаляющая лишние нули и пробелы
string* parse_entry(string entry, Table* table);

int main() {
	setlocale(LC_ALL, "Russian");
	locale::global(locale("Russian_Russia.1251"));
	system("chcp 1251 > NUL");
	// Инициализируем потоки для записи логов
	init_logs_streams();
	write_to_log("main", "Successfully initialized streams for logs");
	// Теперь открываем заголовок, считываем и строим словарь таблиц и словарь размеров типов
	write_to_log("main", "parse_header called:");
	parse_header();
	// Переходим к главному циклу - обработчику запросов пользователя
	write_to_log("main", "Successfully reached main loop");
	chrono::time_point last_call = chrono::system_clock::now();
	while (true) {
		/* Длинная строка, которая всего лишь проверяет сколько времени прошло с времени
		   последнего реального удаления записей в таблицах */
		if (chrono::duration_cast<chrono::minutes>(chrono::system_clock::now() - last_call) >= chrono::minutes(5)) {
			// Каждые пять минут происходит физическое удаление записей во всех таблицах
			clear_deleted();
			last_call = chrono::system_clock::now();
		}
		cout << "Wating for the command:" << endl;
		cin >> input;
		if (input == "EXIT") {
			write_to_log("main", "Exit called");
			break;
		}
		else if (input == "SELECT") Select();
		else if (input == "EDIT") Edit();
		else if (input == "DELETE") Delete();
		else if (input == "DELETE*") clear_deleted();
		else if (input == "ADD") Add();
		else if (input == "EXAMS") Exams();
		else if (input == "SELECTED") Selected();
		else if (input == "UNSELECT") Unselect();
		else {
			cout << "No such command" << endl;
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
	}
	// Удаляем все записи физически, чтобы при следующем запуске они не существовали
	clear_deleted();
	// Закрываем потоки для записи логов
	close_logs_streams();
	return 0;
}

void parse_header() {
	in.open(PATH_TABLES + "header.txt");
	// Очередная и предыдущая строка
	string str, prev = " ";
	// -1 - не определено, 0 - заполнение словаря размеров типов данных, 1 - заполнение словаря таблиц
	int mode = -1;
	// Для заполнения table_names
	string names = ""; int p = 0;
	// Вообще тут так и просится while(true), но нельзя.
	while (str != prev) {
		prev = str;
		getline(in, str);
		if (str == prev) break;
		// Есть возможность писать комментарии через // 
		if (str[0] == '/' && str[1] == '/') {
			write_to_log("main", "Found comment", 1);
			continue;
		}
		// Блок условий переключения режимов
		if (str == "SIZE OF TYPES") {
			write_to_log("main", "Switched to reading size of types", 1);
			mode = 0;
			continue;
		}
		if (str == "TABLES") {
			write_to_log("main", "Switched to reading tables", 1);
			mode = 1;
			continue;
		}
		if (str == "KEY ATTRIBUTES") {
			write_to_log("main", "Switched to reading key attributes", 1);
			mode = 2;
			continue;
		}
		// Блок действий, соответствующих режиму
		if (mode == 0) {
			string type = string(str.substr(0, str.find(':')));
			int size = stoi(str.substr(str.find(':') + 1, string::npos));
			write_to_log("main", "Adopted new type " + type + ", size " + to_string(size), 1);
			size_of_types[type] = size;
		}
		else if (mode == 1) {
			// Создаём экземпляр класса Table
			string attributes, types;
			getline(in, attributes);
			getline(in, types);
			write_to_log("main", "Table constructor called with name " + str, 1);
			Table* some_table = new Table(str, attributes, types);
			// Убираем таким образом расширение файла и записываем в словарь
			str = string(str.substr(0, str.find('.')));
			tables[str] = some_table;
			names += str + ";"; amount_tables++;
		}
		else if (mode == 2) {
			int index = (int)str.find(';');
			string table_name = string(str.substr(0, str.find(':')));
			// Если ключевой атрибут у таблицы один (как, например, у specialities)
			if (index == string::npos) {
				key_attributes[table_name][0] = string(str.substr(str.find(':') + 1, string::npos));
				key_attributes[table_name][1] = FLAG;
			}
			else {
				key_attributes[table_name][0] = string(str.substr(str.find(':') + 1, index - str.find(':') - 1));
				key_attributes[table_name][1] = string(str.substr(index + 1, string::npos));
			}
			write_to_log("main", "Adopted key attributes for table " + table_name + ": " +
				key_attributes[table_name][0] + ", " + key_attributes[table_name][1], 1);
		}
	}
	// Заполняем словарь вида: *имя таблицы* : *указатель на экземпляр класса table*
	table_names = new string[amount_tables];
	for (int i = 0; i < amount_tables; i++) {
		table_names[i] = names.substr(p, names.find(';', p) - p);
		p += (int)table_names[i].length() + 1;
	}
	write_to_log("main", "Successfully parsed header");
	return;
}

void init_logs_streams() {
	// Флаг ios::trunc означает, что файл очищается перед записью
	out_table_hash.open(LOG_PATH_TABLE_HASH, ios::out | ios::trunc);
	out_main.open(LOG_PATH_MAIN, ios::out | ios::trunc);
	out_table.open(LOG_PATH_TABLES, ios::out | ios::trunc);
	write_to_log("main", "Successfully opened logs files");
	return;
}

void close_logs_streams() {
	out_table_hash.close();
	out_table.close();
	write_to_log("main", "Successfully closed non-main log streams");
	out_main.close();
	return;
}

string* parse_entry(string entry, Table* table) {
	string att, * ans = new string[table->amount_of_columns];
	int p = 0, i = 0;
	while (entry.find(';', p) != string::npos) {
		att = entry.substr(p, entry.find(';', p) - p);
		p += (int)att.length() + 1;
		if (table->types[i] != "int") {
			int k = (int)att.length() - 1; while (att[k] == ' ') k--;
			att.erase(k + 1, string::npos);
		}
		// Да простит меня Игорь Михайлович за этот синтаксический ужас 
		// ( while(att[0] == '0') att.erase(0); делает то же самое )
		else att = to_string(stoi(att));
		ans[i++] = att;
	}
	return ans;
}

long int get_hash(string something) {
	// результат и переменная для оптимизации условия в for
	unsigned long int res = 0, len = (unsigned long int)something.length();
	// Для строк длиной меньше 20 используем накопление степеней, в противном случае pow
	if (len < 20) {
		// Переменная накопления степеней C (быстрее, чем pow)
		unsigned long long int degree_C = 1;
		for (unsigned int i = 0; i < len; i++) {
			res += unsigned long int(int(something[i]) * degree_C);
			res %= REMAINDER;
			degree_C *= C;
		}
	}
	else {
		for (unsigned int i = 0; i < len; i++) {
			// Приведение к unsigned long long не является проблемой, переполнения не происходит
			res += int(something[i]) * ((unsigned long long int)pow(C, i) % REMAINDER);
			res %= REMAINDER;
		}
	}
	return res;
}

/* Строка out указывает, в какой именно лог - файл должна быть произведена запись, 
   строка message - собственно сама запись, indent - число табуляций (если не указать,
   по умолчанию 0, нужно для повышения читабельности самих логов) */
void write_to_log(string out, string message, int indent) {
	// Получаем текущее локальное время в формате час:минута:секунда
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t now_c = chrono::system_clock::to_time_t(now); tm local_time;
	localtime_s(&local_time, &now_c);
	// В начале очередной записи в лог добавляем время
	if (out == "main") {
		out_main << put_time(&local_time, "%H:%M:%S") <<
			" | " + string(4 * indent, ' ') + message << endl;
	}
	else if (out == "table_hash") {
		out_table_hash << put_time(&local_time, "%H:%M:%S") <<
			" | " + string(4 * indent, ' ') + message << endl;
	}
	else if (out == "tables") {
		out_table << put_time(&local_time, "%H:%M:%S") <<
			" | " + string(4 * indent, ' ') + message << endl;
	}
}