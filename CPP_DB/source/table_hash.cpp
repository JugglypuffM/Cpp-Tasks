#include "main.h"
// Маленькая библиотека для перевода адреса текущей таблицы в string и записи в лог
#include <sstream>

// Обработчик критических ситуаций для действий, произведённых в этом исходнике
void exception_table_hash() {
	write_to_log("main", "Exception in table_hash.cpp; Check table_hash.log for details");
	exit(EXIT_FAILURE);
}

// Это нужно, чтобы для столбцов типа int хэшем было собственно значение
void Table_hash::add(string str, long int index) {
	add(get_hash(str), index);
}

// Метод, добавляющий элемент типа index_item в массив index_table
void Table_hash::add(long int hash, long int index) {
	// Инициализация
	if (n == 0) {
		// Получаем адрес в памяти в формате HEX текущей таблицы и заполняем соответствующее поле
		// (нужно для записи логов, чтобы отличать таблицы друг от друга)
		stringstream stream;
		stream << this;
		address = string(stream.str());
		// malloc. А позже и realloc с memmove, т. к. vector запретили :(
		index_table = (index_item*)malloc(sizeof(index_item));
		// Таблицу проинициализировали, стало быть, надо заполнить нулевой элемент
		index_table[0].hash = hash;
		index_table[0].index = index;
		index_table[0].deleted = false;
		// Чуть-чуть записей в логи никогда не будет лишним
		write_to_log("table_hash", "Initialized table with address " + address);
		n++;
	}
	// Добавление
	else {
		/* Прежде чем добавлять запись, следует проверить: а нет ли у нас уже такой записи?
			Ведь легко представить себе ситуацию, когда запись сначала удаляется, а потом
			вновь добавляется. Тогда, чтобы index оставался однозначным определителем элемента
			index_table, нужно "активировать" "удалённую" запись. Также это спасёт от дубликатов.
			Но это доп. операции, да. Пора смириться, что add работает долго. */
		int some_n; long int* matches = get_indexes(hash, &some_n, true); bool found = false;
		// Вообще, если some_n != 0, то он всегда = 1. Перечитай комментарий выше, если не понял.
		if (some_n != 0) {
			if (matches[0] == index) {
				bool changed = false;
				int ind = find_elem_with_index(index);
				if (ind != -1) {
					changed = (index_table[ind].deleted ? true : false);
					index_table[ind].deleted = false;
				}
				write_to_log("table_hash", "Tried to add elem with hash " + to_string(hash) + " and index "
					+ to_string(index) + " at table with address " + address + " but already had same elem, "
					+ (changed ? "activated" : "did nothing"));
				delete[] matches;
				found = true;
			}
		}
		delete[] matches;
		if (!found) {
			// Перевыделяем память методом прямиком из pure С
			// Определяем позицию, куда вставить элемент
			long int pos = find_position(hash);
			n++;
			index_table = (index_item*)realloc(index_table, n * sizeof(index_item));
			// Если это не последний элемент, то "раздвигаем" таблицу
			if (pos != n - 1) {
				// Древняя магия, почти как malloc и realloc
				memmove((void*)((unsigned long long)index_table + (pos + 1) * sizeof(index_item)),
					(void*)((unsigned long long)index_table + pos * sizeof(index_item)),
					(n - pos - 1) * sizeof(index_item));
			}
			// И заполняем поля у "освободившейся" ячейки таблицы
			index_table[pos].hash = hash;
			index_table[pos].index = index;
			index_table[pos].deleted = false;
			// И, опять же, запись в лог
			write_to_log("table_hash", "Added element to table with address " + address + " with hash "
				+ to_string(hash) + " and index " + to_string(index) + " at pos " + to_string(pos));
		}
	}
}

// Метод для delete_elem и "активации" уже существующей записи в add. Ищет элемент по индексу.
// И да, однозначно задать элемент таблицы можно только индексом (не хэшом).
long int Table_hash::find_elem_with_index(long int index) {
	// Тут никаких трюков с бинпоиском, к сожалению, не выйдет, т. к. таблица упорядочена по хэшу.
	for (long int i = 0; i < n; i++)
		if (index_table[i].index == index)
			return i;
	return -1;
}

// Метод для "удаления" элемента таблицы. В кавычках потому что, напомню, никакого удаления не происходит
// (см. struct index_item). С радостью бы назвал метод delete, да нельзя, перегрузка...
void Table_hash::delete_elem(long int index) {
	index_table[find_elem_with_index(index)].deleted = true;
	write_to_log("table_hash", "Deleted element with index " + to_string(index) + " in table with address " + address);

}
// Комментарий
void Table_hash::activate_elem(long int index) {
	index_table[find_elem_with_index(index)].deleted = false;
	write_to_log("table_hash", "Activated element with index " + to_string(index) + " in table with address " + address);
}

bool Table_hash::check_deleted(int index) {
	return index_table[find_elem_with_index(index)].deleted;
}
// Функция, меняющая хэш на новый у заданного индексом элемента
void Table_hash::replace_hash(int index, int new_hash) {
	delete_elem(index); add(new_hash, index);
}

// Находит позицию, куда вставить очередной элемент так, чтобы не нарушить упорядоченность по убыванию
long int Table_hash::find_position(long int hash) {
	// Немного хардкода, очень полезного на начальных стадиях заполнения таблицы
	if (index_table[0].hash < hash) return 0;
	if (index_table[n - 1].hash > hash) return n;
	long int l = 0, r = n - 1, m, i = 0;
	/* i тут нужно только для исключения ситуации "повисшего" while, но, в теории, он никогда не должен виснуть.
	   Для дальнейшего понимания очень важно знать, что новый элемент встанет на позицию ДО элемента,
	   чей индекс возвращается, т. е. m-тый элемент двигается вместе с нижней (правой) частью таблицы. */
	while (i < 100000) {
		m = ((r + l) >> 1);
		// Если нашли равный элемент, то при вставке до него упорядоченность не нарушится
		if (index_table[m].hash == hash) return m;
		if (index_table[m].hash > hash && m == n - 1) return n;
		if (index_table[m].hash < hash && m == 0) return 0;
		// Если текущий элемент больше, а следующий уже меньше, то место найдено
		if (index_table[m].hash > hash && index_table[m + 1].hash < hash) return m + 1;
		// Не забываем, что все предыдущие условия не выполнены. Сдвиги поиска
		if (index_table[m].hash < hash) r = m;
		else l = m + 1;
		i++;
		// Кстати, удалён ли элемент тут ничего не значит, как вы заметили.
	}
	// Если всё таки каким-то чудом получилось, что while "повис", то вот обработчик
	write_to_log("table_hash", "Critical Error: Couldn't find position at table with address "
		+ address + ", hash: " + to_string(hash));
	exception_table_hash();
	return -1;
}

int Table_hash::find_hash(int hash) {
	if (n == 0) return -1;
	if (index_table[0].hash < hash || index_table[n-1].hash > hash) return -1;
	if (index_table[0].hash == hash) return 0;
	if (index_table[n - 1].hash == hash) return n - 1;
	int l = -1, r = n - 1, m, i = 0;
	while (i < 100000) {
		m = ((r + l) >> 1);
		if (index_table[m].hash == hash) return m;
		else if (index_table[m].hash < hash) r = m;
		else l = m;
		i++;
	}
	return -1;
}
// Возвращает ВСЕ индексы, соответствующие переданному хэшу. Длина массива записывается по указателю.
long int* Table_hash::get_indexes(long int hash, int* len, bool include_deleted) {
	*len = 0;
	// Да, тут уже можно было бы использовать list (напомню, vector под запретом), но однажды
	// научившись использовать malloc и realloc, возвращаться к синтаксическому сахару не будешь (ауф).
	long int* res = (long int*)malloc(sizeof(long int));
	long int pos = find_hash(hash);
	// Если это условие выполняется, то такого хэша просто нет в таблице, возвращаем пустой res и len = 0.
	if (pos > n - 1 || pos < 0) return res;
	if (index_table[pos].hash != hash) return res;
	// (чтобы понять, почему так, см. find_position)
	/*  Тут важно понимать, что find_position находит абсолютно любую позицию, такую что упорядоченность
		при добавлении нового элемента не нарушится. То есть если у нас три подряд одинаковых хэша, она
		спокойно может попасть в тот, что в середине. Поэтому: */
	// Уходим влево (вверх) по записям таблицы до тех пор, пока не пройдём все, чей хэш равен искомому.
	if (pos - 1 > 0) { 
		while (index_table[pos - 1].hash == hash) {
			if (pos - 1 > 0) pos--;
			else break;
		}
		if (index_table[pos].hash != hash) pos++;
	}
	// Идём вправо (вниз) и записываем в результат все вхождения.
	while (index_table[pos].hash == hash) {
		// Если текущий элемент не "удалён", то он подходит.
		if (!index_table[pos].deleted || include_deleted) {
			// Перевыделяем память и записываем в последний элемент новое вхождение.
			*len = *len + 1;
			res = (long int*)realloc(res, *len * sizeof(long int));
			res[*len - 1] = index_table[pos].index;
		}
		pos++;
	}
	return res;
}

// Функция для отладки. Выводит содержимое хэш-таблицы в лог.
void Table_hash::print_table() {
	write_to_log("table_hash", "print_table called on table with address " + address + "\n");
	if (this->n == 0) {
		write_to_log("table_hash", "empty", 1);
		return;
	}
	write_to_log("table_hash", " Hash  Index Deleted", 1);
	for (int i = 0; i < n; i++)
		write_to_log("table_hash", to_string(index_table[i].hash) + "   " + to_string(index_table[i].index)
			+ "   " + (index_table[i].deleted ? "True" : "False"), 1);
	write_to_log("table_hash", "End of print_table");
}