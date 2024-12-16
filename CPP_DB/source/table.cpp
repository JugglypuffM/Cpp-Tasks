#include "main.h"
// для remove и rename в physically_clear_entries
#include <cstdio>

// Обработчик критических ситуаций для действий, произведённых в этом исходнике
void exception_table() {
	cout << "Exception in table.cpp; Check tables.log for details";
	exit(EXIT_FAILURE);
}

// "Магическая" функция, которая "добивает" переданную строку до необходимого количества байт 
string make_correct_size(string str, string type) {
	if (type == "int") return string(size_of_types[type] - str.length(), '0') + str + ";";
	else return str + string(size_of_types[type] - str.length(), ' ') + ";";
}

// Функция, выбирающая лучший атрибут для хэш-поиска
string get_best_attribute_to_hash(Table* table, string* atts, int* best_i, bool* integer);

// Конструктор класса Table. Принимает имя (путь до файла), атрибуты и типы через ';'
Table::Table(string name, string atts, string types) {
	if (name.empty() || atts.empty() || types.empty()) {
		write_to_log("tables", "Couldn't initialize table " + (name.empty() ? "" : name) + ". Check header.txt");
		exception_table();
	}
	write_to_log("tables", "Table constructor called with name " + name);
	this->path = name;
	// Количество разделителей равно количеству атрибутов (count определена в main.cpp)
	this->amount_of_columns = count(atts, ';');
	write_to_log("tables", "Found " + to_string(amount_of_columns) + " columns", 1);
	// Не забываем, что символ ';' так же занимает 1 байт
	this->size_of_entry += amount_of_columns;
	// Выделяем память для массива атрибутов и типов
	this->attributes = new string[amount_of_columns]; this->types = new string[amount_of_columns];
	// Параллельно разбиваем строки atts и types по ';'
	int p1 = 0, p2 = 0, to_be_hashed = 0;
	string att, type;
	for (int i = 0; i < amount_of_columns; i++) {
		// "Срезаем" очередной атрибут и тип этого атрибута, заполняем массивы
		att = atts.substr(p1, atts.find(';', p1) - p1);
		p1 += (int)att.length() + 1;
		this->attributes[i] = att;
		type = types.substr(p2, types.find(';', p2) - p2);
		p2 += (int)type.length() + 1;
		write_to_log("tables", "Adopted attribute " + att + " with type " + type, 1);
		// Не забываем так же про size_of_entry
		if (size_of_types[type] == 0) {
			write_to_log("tables", "Couldn't find size of type " + type + ". Check header.txt");
			exception_table();
		}
		this->size_of_entry += size_of_types[type];
		this->types[i] = type;
		// Если перед нами тип, по которому нужно строить хэш-таблицу
		if (type == "int" || type == "short_str" || type == "long_str") {
			// то маркируем это в соответствующем словаре
			this->hashed[att] = to_be_hashed++;
		}
		else this->hashed[att] = -1;
	}
	write_to_log("tables", "Size of entry: " + to_string(size_of_entry), 1);
	// Выделяем память под хэш-таблицы
	this->hash_tables = new Table_hash * [to_be_hashed];
	for (int i = 0; i < to_be_hashed; i++) this->hash_tables[i] = new Table_hash();
	// И строим их в отдельной функции
	this->init_hash_tables();
	write_to_log("tables", "Completed initialization of table with name " + name);
}

// Функция, считывающая связанный с таблицей файл и строящая хэш-таблицы по столбцам (где возможно)
void Table::init_hash_tables() {
	write_to_log("tables", "Started initializing hash tables", 1);
	ifstream in(PATH_TABLES + path);
	// Немного неправильно делать это отсюда, конечно, но тем не менее
	if (!in.is_open()) {
		write_to_log("tables", "Couldn't open table " + path + ". Check header.txt");
		exception_table();
	}
	// prev нужен для проверки, закончился ли файл. entry и att - очередная запись и атрибут
	string prev, entry, att;
	// Индекс текущей записи в файле (0 - первая строка, 1 - вторая и т. д.)
	int i = 0;
	// По поводу while(true). Давайте просто признаем, что файлы имеют свойство заканчиваться.
	while (true) {
		getline(in, entry);
		// И что это условие достаточно корректно, чтобы исключить "повисший" while.
		if (entry == prev || entry.empty()) break;
		// Попутно считаем общее количество записей в таблице
		n++;
		prev = entry;
		// p - нужно для сдвига разбиения строки на атрибуты, j - номер текущего атрибута
		int p = 0, j = 0;
		// Думаю, условие исчерпывающее
		while (entry.find(';', p) != string::npos) {
			// Считываем очередной атрибут в текущей записи
			att = entry.substr(p, entry.find(';', p) - p);
			p += (int)att.length() + 1;
			// Если по текущему атрибуту не нужно строить хэш-таблицу, то пропускаем
			if (hashed[attributes[j]] == -1) continue;
			// Если перед нами строка
			if (types[j] != "int") {
				// убираем все пробелы с конца
				int k = (int)att.length() - 1;
				while (att[k] == ' ') k--;
				att.erase(k + 1, string::npos);
				// И уже без пробелов добавляем значение в соответствующую хэш-таблицу
				hash_tables[j]->add(att, i);
			}
			/* Если перед нами число, то убираем лишние нули (как пробелы для строк) при помощи stoi
			   и так же добавляем в соответствующую хэш-таблицу. Напомню, что для столбцов типа int
			   хэшом служит само значение. */
			else hash_tables[j]->add(stoi(att), i);
			j++;
		}
		i++;
	}
	write_to_log("tables", "Successfully initialized hash tables, found " + to_string(n) + " entries", 1);
	in.close();
}

// Функция, которая возвращает массив атрибутов записи с указанным индексом
string* Table::get_entry(int index) {
	write_to_log("tables", "Called get_entry on table " + path + " and entry " + to_string(index));
	// Открываем связанный с таблицей файл и сдвигаем указатель считывания
	ifstream in(PATH_TABLES + path);
	// 2 * index это переносы строки (2 байта в кодировке windows-1251)
	in.seekg(index * size_of_entry + 2 * index, ios::beg);
	// Считываем строку
	string entry, *ans;
	getline(in, entry);
	in.close();
	// Разбиваем строку на атрибуты
	ans = parse_entry(entry, this);
	write_to_log("tables", "Successfully parsed and returned entry");
	return ans;
}

string** Table::get_all_entries() {
	// Надо иметь ввиду, что хоть и попросили ВСЕ записи, удалённые мы всё таки не должны возвращать
	int amount = 0, deleted = 0; 
	for (int i = 0; i < n; i++) if (!hash_tables[0]->check_deleted(i)) amount++;
	string** ans = new string * [amount], entry;
	ifstream in(PATH_TABLES + path);
	for (int i = 0; i < n; i++) {
		getline(in, entry);
		if (!hash_tables[0]->check_deleted(i))
			ans[i - deleted] = parse_entry(entry, this);
		else deleted++;
	}
	in.close();
	return ans;
}

// Функция, "удаляющая" запись с переданным индексом
void Table::delete_entry(int index) {
	/* По факту, конечно, никакого удаления не происходит, иначе нужно было бы переиндексировать все
	Table_hash'ы. Вместо этого просто ставится в true флаг deleted у элемента, соответствующего этой записи, 
	во всех Table_hash'ах. Это убережёт от того, что "удалённая" запись будет найдена и оставит соответствие. 
	Тем не менее, настоящее удаление происходит*/
	for (int i = 0; i < amount_of_columns; i++)
		if (hashed[attributes[i]] != -1) hash_tables[hashed[attributes[i]]]->delete_elem(index);
	// А это флаг, который отвечает за то, была ли удалена хотя бы одна запись с момента последней очистки
	deleted = true;
	write_to_log("tables", "Successfully matched entry " + to_string(index) + " as deleted on table" + path);
}

// Достаточно тяжёлая функция, физически очищающая записи из соответствующего таблице файла и переиндексирующая хэш-таблицы
void Table::physically_clear_entries() {
	write_to_log("tables", "Started physically clearing deleted entries on table " + path);
	// Мне показалось очень разумным сделать флаг у таблицы, отвечающий за то, была ли удалена хотя бы одна
	// запись. Просто проделывать всю процедуру ниже впустую - звучит очень уж плохо.
	if (!deleted) {
		write_to_log("tables", "Nothing to delete");
		return;
	}
	// Для удобства размер строки вынесен в отдельную переменную
	const int row_size = size_of_entry + 2;
	// Наилучшим вариантом будет создать копию текущей таблицы, пропустив "удалённые" строки, затем
	// удалить оригинал и переименовать копию.
	ifstream original(PATH_TABLES + path);
	// конструктор ofstream подразумевает, что если файл не существует, то он будет создан
	ofstream copy(PATH_TABLES + "temp.db");
	// temp - очередная строка при считывании оригинальной таблицы
	string temp;
	for (int i = 0; i < n; i++) {
		getline(original, temp);
		// Если текущая строка удалена, то во всех хэш-таблицах по этому индексу она так же будет удалена
		if (hash_tables[0]->check_deleted(i)) continue;
		// Если же строка не удалена, то записываем её в файл-копию
		copy << temp << endl;
	}
	original.close(); copy.close();
	// Удаляем оригинальный файл таблицы
	remove(string(PATH_TABLES + path).c_str());
	// Переименовываем копию так, как назывался оригинал.
	rename(string(PATH_TABLES + "temp.db").c_str(), string(PATH_TABLES + path).c_str());
	// "Сбрасываем" хэш-таблицы и поле n, и перечитываем записи заново
	n = 0; deleted = false;
	for (int i = 0; i < amount_of_columns; i++) {
		if (hashed[attributes[i]] != -1) hash_tables[hashed[attributes[i]]] = new Table_hash();
	}
	write_to_log("tables", "Successfully physically cleared deleted entries, now calling init_hash_tables again");
	init_hash_tables();
	return;
}

/* Очень тяжёлая функция, так как она изменяет и Table_hash'ы, и сам файл. Для дальнейшего понимания важно,
   что new_atts устроен так же, как и string* atts в get_entries_matching, т. е. элементов всегда столько же,
   сколько и в таблице, и те атрибуты, значение которых изменять не нужно, маркируются тем же флагом. */
void Table::edit_entry(int index, string* new_atts) {
	write_to_log("tables", "Edit entry called on table " + path + " at entry " + to_string(index));
	// Сначала получаем запись такой, какой она была, чтобы соединить с новыми атрибутами
	string* how_it_was = get_entry(index);
	// Строка, в которую будет записываться новая запись
	string new_entry = "";
	;	for (int i = 0; i < amount_of_columns; i++) {
		// Если текущий атрибут - флаг, то оставляем его таким, какой он был
		if (new_atts[i] == FLAG) {
			// функция make_correct_size "добивает" строку до определённого количества байт (зависит от типа)
			new_entry += make_correct_size(how_it_was[i], types[i]);
			write_to_log("tables", "Found flag, skipped", 1);
			continue;
		}
		write_to_log("tables", "Changed attribute from " + how_it_was[i] + " to " + new_atts[i], 1);
		// Если текущий атрибут хэшируется, то нужно обновить значение хэша на новое
		if (hashed[attributes[i]] != -1) {
			if (types[i] == "int") hash_tables[hashed[attributes[i]]]->replace_hash(index, stoi(new_atts[i]));
			else hash_tables[hashed[attributes[i]]]->replace_hash(index, get_hash(new_atts[i]));
			write_to_log("tables", "Replaced hash in corresponding hash table", 2);
		} else write_to_log("tables", "This column is non-hashed, did nothing", 2);
		// И, поскольку первое условие не выполнено, значит записываем в новую запись новый атрибут
		new_entry += make_correct_size(new_atts[i], types[i]);
	}
	// Поток и на чтение, и на запись, для замены строки
	fstream stream(PATH_TABLES + path, ios::in | ios::out);
	// Вычисляем сдвиг в байтах до текущей записи
	int pos = index * size_of_entry + 2 * index;
	stream.seekg(pos);
	stream.seekp(pos);
	// Записываем новую строку
	stream.write(new_entry.c_str(), size_of_entry);
	stream.close();
	// Не забываем про утечки памяти
	delete[] how_it_was;
	write_to_log("tables", "Successfully edited entry");
}

// Массив atts тут устроен так же, как и в edit_entry и get_entries_matching
void Table::add_entry(string* atts) {
	// Сначала нужно проверить, нет ли уже такой записи, и если она есть и "удалена", то "активировать"
	int n_matches, *matches = get_entries_matching(this, atts, &n_matches, true);
	if (n_matches > 0) {
		for (int i = 0; i < n_matches; i++) {
			for (int j = 0; j < amount_of_columns; j++)
				if (hashed[attributes[j]] != -1)
					hash_tables[hashed[attributes[j]]]->activate_elem(matches[i]);
		}
		// Не забываем про утечки памяти
		delete[] matches;
		write_to_log("tables", "Called add_entry at table with name " + path + ", but already had same entry, so activated");
	}
	else {
		// Строка для соединения атрибутов и последующей записи в файл
		string entry = "";
		for (int i = 0; i < amount_of_columns; i++) {
			// "Добиваем" очередной атрибут до нужного количества байт
			entry += make_correct_size(atts[i], types[i]);
			// Добавляем в хэш-таблицу новый элемент, если хэш-таблица по текущему столбцу строится
			if (hashed[attributes[i]] != -1) {
				if (types[i] == "int") hash_tables[hashed[attributes[i]]]->add(stoi(atts[i]), n);
				else hash_tables[hashed[attributes[i]]]->add(atts[i], n);
			}
		}
		// Поток на запись, ios::app указывает, что запись необходимо осуществить в конец файла
		ofstream out(PATH_TABLES + path, ios::app);
		out << entry << endl;
		out.close();
		n++;
		write_to_log("tables", "Successfully added new entry to table with name " + path);
	}
}

/* Ключевая функция.Ищет в таблице все записи, у которых атрибуты равны заданным(соответствуют запросу)
   Возвращает массив индексов соответствующих записей в таблице, длина записывается по указателю *len.
   Если *len = 0 то ничего не найдено. Если *len = -1 то был передан некорректный запрос.
   Для дальнейшего понимания важно, что в atts всегда столько элементов, сколько столбцов у таблицы, поля,
   значение которых неважно (т. е. любое), заполнены константой FLAG, инициализированной в main.cpp */
int* get_entries_matching(Table* table, string* atts, int* len, bool include_deleted) {
	// Массив возвращаемых значений - индексов записей в таблице
	*len = 0;
	int* ans = 0, best_i; bool integer;
	write_to_log("tables", "Called get_entries_matching at table " + table->path);
	string best = get_best_attribute_to_hash(table, atts, &best_i, &integer);
	// Стало очень лень ещё раз писать malloc и realloc, сделал через строку и delimiter = ';'
	string answer = "";
	// Теперь по хэшу "лучшего" атрибута получаем все записи, претендующие на полное совпадение запросу.
	int n_matches;
	long int* matches = table->hash_tables[best_i]->get_indexes(
		(integer ? stoi(best) : get_hash(best)), &n_matches, include_deleted);
	write_to_log("tables", "Found " + to_string(n_matches) + " entries from best hash");
	// Важно, что при проверке НЕ пропускается атрибут, по хэшу которого были получены "кандидаты" 
	// ведь хэш-функция может иметь коллизии. Если вдруг поле в таблице не заполнено, то оно просто пропускается
	bool flag; string* entry = 0;
	for (int i = 0; i < n_matches; i++) {
		entry = table->get_entry(matches[i]);
		flag = true;
		for (int j = 0; j < table->amount_of_columns; j++) {
			if (atts[j] != FLAG && entry[j] != FLAG && atts[j] != entry[j]) {
				flag = false;
				break;
			}
		}
		if (flag) {
			answer += to_string(matches[i]) + ';';
			*len += 1;
		}
	}
	write_to_log("tables", to_string(*len) + " entries matching the conditions");
	// Не забываем про утечки, освобождаем память, выделенную под entry и matches
	delete[] entry; delete[] matches;
	if (answer == "") *len = 0;
	else {
		ans = new int[*len]; int p = 0;
		string smth;
		for (int i = 0; i < *len; i++) {
			smth = answer.substr(p, answer.find(';', p) - p);
			p += (int)smth.length() + 1;
			ans[i] = stoi(smth);
		}
	}
	return ans;
}

string get_best_attribute_to_hash(Table* table, string* atts, int* best_i, bool* integer) {
	// Выбираем "лучший" атрибут для хэш-поиска. Это должна быть строка, и чем длиннее, тем лучше.
	// Но, естественно, не всегда в запросе будет строка, поэтому флаг integer
	string best = ""; *integer = false;
	for (int i = 0; i < table->amount_of_columns; i++) {
		if (atts[i].empty()) {
			write_to_log("tables", "Found empty attribute in get_best_attribute_to_hash call at table " +
				table->path + ". Execution continued.");
			continue;
		}
		if (atts[i] == FLAG) continue;
		if (best == "") {
			best = atts[i];
			*best_i = i;
			if (table->types[i] == "int") *integer = true;
			continue;
		}
		if (table->types[i] == "short_str" || table->types[i] == "long_str") {
			if (*integer) {
				best = atts[i];
				*best_i = i;
				*integer = false;
				continue;
			}
			if (best.length() < atts[i].length()) {
				best = atts[i];
				*best_i = i;
			}
		}
	}
	if (best == "") {
		write_to_log("tables", "Got empty request at table " + table->path +
			" while parcing at get_best_attribute_to_hash, cannot continue execution");
		exception_table();
	}
	return best;
}

// Функция для отладки, выводит таблицу в лог-файл
void Table::print_table() {
	write_to_log("tables", "print_table called on table with name " + path);
	write_to_log("tables", "");
	write_to_log("tables", "n : " + to_string(amount_of_columns), 1);
	write_to_log("tables", "size of entry : " + to_string(size_of_entry), 1);
	write_to_log("tables", "attribute : type", 1);
	string is_hashed = "", non_hashed = "";
	write_to_log("table_hash", "External calls from table with name " + path);
	for (int i = 0; i < n; i++) {
		write_to_log("tables", attributes[i] + " : " + types[i], 1);
		if (hashed[attributes[i]] != -1) {
			is_hashed += attributes[i] + ", ";
			hash_tables[i]->print_table();
		}
		else non_hashed += attributes[i] + ", ";
	}
	write_to_log("table_hash", "End of external calls from table with name " + path);
	if (is_hashed != "") is_hashed.erase(is_hashed.length() - 2, 2);
	if (non_hashed != "") non_hashed.erase(non_hashed.length() - 2, 2);
	write_to_log("tables", "hashed attributes : " + (is_hashed == "" ? "none" : is_hashed), 1);
	write_to_log("tables", "non-hashed attributes : " + (non_hashed == "" ? "none" : non_hashed), 1);
	write_to_log("tables", "calling print_table for all hash tables, check table_hash.log", 1);
	write_to_log("tables", "End of print_table on table with name " + path);
}