#include "main.h"
#include <codecvt>

string name, selected_name, group, exam;
int selected_amount = NULL, *selected_indexes = nullptr;
// Номера записи для выполнения операции
int edit_number, edit_index;

void exception_operations(string message) {
	write_to_log("main", message);
	exit(EXIT_FAILURE);
}

// Валидация аттрибутов, пока не подадут корректные
string* validated_atts() {
	write_to_log("main", "Validating attributes", 1);
	string raw_attributes;
	string* attributes = new string[tables[name]->amount_of_columns];
	// Читаем строку из атрибутов, разделенных ';'
	cin.clear();
	cin.sync();
	getline(cin, raw_attributes);
	while (raw_attributes[0] == ' ') raw_attributes.erase(0, 1);
	// Ключевое слово для выделения всех записей
	if (raw_attributes == "ALL") {
		string* attributes = new string[tables[name]->amount_of_columns];
		for (int i = 0; i < tables[name]->amount_of_columns; i++) attributes[i] = FLAG;
		write_to_log("main", "ALL-word was given", 1);
		return attributes;
	}
	// парсер возвращает nullptr, если аттрибутов получилось меньше, чем колонок
	write_to_log("main", "Parsing attributes", 1);
	string current = "";
	int count = 0;
	int len = (int)raw_attributes.length();
	for (int i = 0; i < len; i++) {
		if (count == tables[name]->amount_of_columns) {
			write_to_log("main", "Too many attributes are given", 2);
			cout << "Too many attributes" << endl;
			return nullptr;
		}
		// Находим разделитель -> сохраняем слово и ищем новое
		if (raw_attributes[i] == ';') {
			attributes[count++] = current;
			current = "";
		}
		// В конце делаем то же, только букву надо добавить к слову
		else if (i == len - 1) {
			current += raw_attributes[i];
			attributes[count++] = current;
		}
		// Просто строим слово
		else current += raw_attributes[i];
	}
	// Валидация соответствия количества атрибутов и столбцов в таблице
	if (count == tables[name]->amount_of_columns) {
		write_to_log("main", "Parsed succesfuly", 1);
		for (int i = 0; i < tables[name]->amount_of_columns; i++) {
			if ((tables[name]->types[i] == "int" && !is_int(attributes[i]) && (attributes[i] != FLAG))) {
				write_to_log("main", "Bad attributes are given", 2);
				cout << "Wrong attributes" << endl;
				return nullptr;
			}
		}
		return attributes;
	}
	write_to_log("main", "Bad attributes are given", 2);
	cout << "Wrong attributes" << endl;
	return nullptr;
}

// Ввод имени с проверкой существования соответствующей таблицы, если таковой нет, то ждет другого имени
string name_in(string operation) {
	cin >> name;
	write_to_log("main", operation + " operation in the " + name + " table called");
	// Проверка существования таблицы с таким именем, если нет, то просьба ввести имя снова
	if (!tables[name]) {
		write_to_log("main", operation + " operation failed, wrong table name was given, waiting for another", 1);
		cout << "No table with such name" << endl;
		getline(cin, name);
		return "";
	}
	write_to_log("main", "Correct name recieved, continuing execution", 1);
	return name;
}

// То же, но для номера
int number_in(string operation) {
	string string_number;
	int number; 
	write_to_log("main", operation + " operation called");
	if (!selected_indexes) {
		write_to_log("main", operation + " operation failed, nothing was selected", 1);
		cout << "Select something first" << endl << endl;
		getline(cin, string_number);
		return -1;
	}
	cin >> string_number; 
	if (!is_int(string_number)) {
		write_to_log("main", operation + " operation failed, not a number was given", 1);
		cout << "Not a number" << endl << endl;
		getline(cin, string_number);
		return -1;
	}
	number = stoi(string_number);
	if ((number <= 0) || (number > selected_amount)) {
		write_to_log("main", operation + " operation failed, number out of range", 1);
		cout << "No entry with such number, try another" << endl << endl;
		getline(cin, string_number);
		return -1;
	}
	return number - 1;
}

// Функция, находящая индекс ключевого атрибута в таблице
int find_att_index(string some_name, string key) {
	int n = tables[some_name]->amount_of_columns;
	for (int i = 0; i < n; i++) if (tables[some_name]->attributes[i] == key) return i;
	exception_operations("Something wrong with key attributes");
}

// Проверка на повторение ключевого атрибута
bool is_key_in_table(string some_name, string* attributes) {
	string key = key_attributes[some_name][0];
	// Узнаем индекс (номер столбца) ключевого атрибута
	int key_index = find_att_index(some_name, key), len_ans = 0;
	int hash = (tables[some_name]->types[key_index] == "int" ? stoi(attributes[key_index]) : get_hash(attributes[key_index]));
	tables[some_name]->hash_tables[tables[some_name]->hashed[key]]->get_indexes(hash, &len_ans);
	if (len_ans != 0) {
		cout << "Attribute in column " + to_string(key_index + 1) + " is key, it cannot be repeated" << endl << endl;
		return true;
	}
	return false;
}

bool is_int(string string_number) {
	for (int i = 0; i < string_number.length(); i++) {
		if ((string_number[i] != '0') && (string_number[i] != '1') && (string_number[i] != '2') &&
			(string_number[i] != '3') && (string_number[i] != '4') && (string_number[i] != '5') &&
			(string_number[i] != '6') && (string_number[i] != '7') && (string_number[i] != '8') &&
			(string_number[i] != '9'))
		{
			return false;
		}
	}
	return true;
}

// Функция для выбора элементов таблицы через поиск по маске вида атрибут1;атрибут2;...;атрибутN, запрашивает имя таблицы и маску
// Если что-либо введено не верное, то печатает соответствующее ообщение и возвращает к вводу команды
// Последнее, как маска атрибутов, аналогично для всех последуюющих, так что дальше повторять смысла нет
void Select() {
	// Ввод имени и атрибутов с валидацией
	name = name_in(input);
	if (name == "") return;
	string* attributes = validated_atts();
	if (!attributes) return;
	int entries_amount, length = tables[name]->amount_of_columns;
	write_to_log("main", "Starting entries search");
	bool defines_every = true;
	write_to_log("main", "Checking defenition of every entry by attributes", 1);
	for (int i = 0; i < length; i++) {
		if (attributes[i] != FLAG) {
			defines_every = false;
			break;
		}
	}
	// Если атрибуты задают все записи, то и выбрать надо все
	if (defines_every) {
		write_to_log("main", "All entries are defined with attributes, writing them down", 1);
		string indexes = "";
		for (int i = 0; i < tables[name]->n; i++)
			if (!tables[name]->hash_tables[0]->check_deleted(i))
				indexes += to_string(i) + ';';
		entries_amount = count(indexes, ';');
		selected_indexes = new int[entries_amount]; int index = (int)indexes.find(';'), j = 0;
		while (index != string::npos) {
			selected_indexes[j++] = stoi(indexes.substr(0, index));
			indexes.erase(0, index + 1);
			index = (int)indexes.find(';');
		}
	}
	// Иначе ищем подходящие
	else {
		write_to_log("main", "Not all entries are defined with attributes, starting search", 1);
		selected_indexes = get_entries_matching(tables[name], attributes, &entries_amount, false);
		// Если неверно заданы аттрибуты
		if (entries_amount == -1) {
			write_to_log("main", "Wrong attributes were given", 1);
			cout << "Wrong attributes were given" << endl << endl;
			return;
		}
		// Если ничего не нашлось
		else if (entries_amount == 0) {
			write_to_log("main", "No entries found", 1);
			cout << "Nothing found" << endl << endl;
			return;
		}
	}
	// Базовый случай с выводом того, что нашли
	write_to_log("main", "Entries selection completed, found " + to_string(entries_amount) + " entries");
	cout << "Selected " + to_string(entries_amount) + " following entries:" << endl << endl;
	selected_name = name;
	selected_amount = entries_amount;
	Selected();
}

// Функция для изменения одной из выбранных записей по маске, запрашивет номер и, собственно, маску
// Номер считается относительно выбранных записей
void Edit() {
	// Вернет -1 если ничего не выбрано
	edit_number = number_in(input);
	if (edit_number == -1) return;
	edit_index = selected_indexes[edit_number];
	string* attributes = validated_atts();
	if (!attributes) return;
	// Проверка как и в ADD
	if (is_key_in_table(name, attributes)) {
		write_to_log("main", "Edition failed, table already contains such key attribute");
		return;
	}
	tables[selected_name]->edit_entry(edit_index, attributes);
	// Лучше убирать выбор при обновлении, мало ли новая запись подпадет под условия
	Unselect();
	delete[] attributes;
}

// Подобно Edit, но для удаления, запрашивет только номер, так же проверяет связи с другими таблицами
// Например, нельзя удалить группу, если в ней состоят студенты
void Delete() {
	// Вернет -1 если ничего не выбрано
	edit_number = number_in(input);
	if (edit_number == -1) return;
	edit_index = selected_indexes[edit_number];
	string* attributes = tables[selected_name]->get_entry(edit_index);
	// Проверка на наличие связей с другими таблицами
	write_to_log("main", "Checking bonds with other tables", 1);
	string** all_entries; string* other_names = new string[amount_tables]; 
	int other_index, others_amount = 0;
	string other_key, local_key = key_attributes[selected_name][0]; 
	int local_index = find_att_index(selected_name, local_key);
	for (int i = 0; i < amount_tables; i++){
		if (key_attributes[table_names[i]][1] == local_key)
			other_names[others_amount++] = table_names[i];
	}
	for (int i = 0; i < others_amount; i++){
		other_index = find_att_index(other_names[i], key_attributes[other_names[i]][1]);
		all_entries = tables[other_names[i]]->get_all_entries();
		int n = tables[other_names[i]]->n;
		for (int j = 0; j < n; j++){
			if (all_entries[j][other_index] == attributes[local_index]) {
				cout << "There are bond attributes in table " + other_names[i] + ", delete or rebond them first" << endl << endl;
				write_to_log("main", "Entries in " + name + " and " + other_names[i] + "tables are bounded, cannot delete");
				return;
			}
		}
	}
	tables[name]->delete_entry(edit_index);
	write_to_log("main", "Deletion complete");
	Unselect();
	delete[] attributes, all_entries, other_names;
}

// Функция для добавления записи в таблицу, запрашивает имя таблицы и маску, которая не может содержать FLAG
// Было бы странно если бы можно было добавить студента с неопределенным именем
// Так же не даст добавить запись с уже существующим в таблице ключевым атрибутом
void Add() {
	// Ввод имени и атрибутов с валидацией
	name = name_in(input);
	if (name == "") return;
	string* attributes = validated_atts();
	if (!attributes) return;
	// Проверка на полноту записи
	bool defines_only = true;
	write_to_log("main", "Checking defenition of only one entry by attributes", 1);
	for (int i = 0; i < tables[name]->amount_of_columns; i++) {
		if (attributes[i] == FLAG) {
			defines_only = false;
			break;
		}
	}
	// Невозможно добавить неполную запись
	if (defines_only) {
		// Проверка на наличие такого ключевого аттрибута в таблице
		if (is_key_in_table(name, attributes)) {
			write_to_log("main", "Addition failed, table already contains such key attribute");
			return;
		}
		// Собственно запись...
		tables[name]->add_entry(attributes);
		write_to_log("main", "Addition comleted");
		Unselect();
	}
	else {
		cout << "Fill all the atributes" << endl << endl;
		write_to_log("main", "Addition failed, attributes were not complete");
	}
	delete[] attributes;
}

// Функция для формирования экзаменационного листа, запрашивает группу и произвольный предмет
// Выводит все в файлик по пути, указанному в таблице students
void Exams() {
	write_to_log("main", "Exam list formation began");
	cin >> group;
	cin.clear();
	cin.sync();
	getline(cin, exam);
	while (exam[0] == ' ') exam.erase(0, 1);
	string* request = new string[tables["groups"]->amount_of_columns];
	for (int i = 0; i < tables["groups"]->amount_of_columns; i++) request[i] = FLAG;
	request[find_att_index("groups", "group_id")] = group;
	int len_ans, *ans = get_entries_matching(tables["groups"], request, &len_ans);
	delete[] request;
	// Если не нашли, то и составлять нечего
	if (len_ans == 0) {
		write_to_log("main", "Exam list formation faild, bad group id");
		cout << "No group with such name\n\n";
		return;
	}
	// Если вдруг нашли больше одной группы по id
	if (len_ans > 1)
		exception_operations("One-to-one mapping of key attribute in groups table broken");
	string* entry = tables["groups"]->get_entry(ans[0]);
	string MEN = entry[find_att_index("groups", "men")], speciality = entry[find_att_index("groups", "id_speciality")];
	// Теперь ищем направление группы и путь к файлу для листа
	request = new string[tables["specialities"]->amount_of_columns];
	for (int i = 0; i < tables["specialities"]->amount_of_columns; i++) request[i] = FLAG;
	request[find_att_index("specialities", "id_speciality")] = speciality;
	ans = get_entries_matching(tables["specialities"], request, &len_ans);
	delete[] request;
	if (len_ans != 1) {
		if (len_ans == 0) {
			cout << "This group has undefined speciality\n\n";
			return;
		}
		else
			exception_operations("One-to-one mapping of key attribute in specialities table broken");
	}
	entry = tables["specialities"]->get_entry(ans[0]);
	string direction = entry[find_att_index("specialities", "direction")], path = entry[find_att_index("specialities", "exam_list")];
	write_to_log("main", "Exam list formated, writing it in " + PATH_TABLES + path);
	// Выводим все в файл
	ofstream out_exams(PATH_TABLES + path);
	out_exams << "Экзамен по предмету: " << exam.c_str() << endl;
	out_exams << "Date: " << endl;
	out_exams << "Direction: " << direction.c_str() << endl;
	out_exams << "Group: MEN-" << MEN.c_str() << endl;
	out_exams << "List of students:" << endl;
	request = new string[tables["students"]->amount_of_columns];
	for (int i = 0; i < tables["students"]->amount_of_columns; i++) request[i] = FLAG;
	request[find_att_index("students", "group_id")] = group;
	ans = get_entries_matching(tables["students"], request, &len_ans);
	delete[] request;
	if (len_ans == 0) out_exams << "No students in this group";
	else {
		string* student;
		for (int i = 0; i < len_ans; i++) {
			out_exams << i + 1 << " ";
			student = tables["students"]->get_entry(ans[i]);
			out_exams << student[find_att_index("students", "surname")].c_str() << " " 
				<< student[find_att_index("students", "name")].c_str() << " " 
				<< student[find_att_index("students", "patronymic")].c_str() << endl;
		}
	}
	write_to_log("main", "Exam list ready");
	out_exams.close();
}

// Печать выбранных записей
void Selected() {
	if (!selected_indexes) {
		cout << "Select something first" << endl << endl;
		return;
	}
	write_to_log("main", "Printing selected entries");
	int length = tables[name]->amount_of_columns;
	for (int i = 0; i < selected_amount; i++) {
		string* current_row = tables[name]->get_entry(selected_indexes[i]);
		cout << i + 1 << " ";
		for (int j = 0; j < length; j++) cout << current_row[j] << " ";
		cout << endl;
	}
	cout << endl;
}

// Отменяет выбор элементов таблицы
void Unselect() {
	selected_amount = NULL;
	selected_name = "";
	selected_indexes = nullptr;
}
