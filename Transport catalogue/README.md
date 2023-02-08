# Описание работы транспортного справочника

Существует две версии основной программы main().

Первая предназначена для ручного ввода данных и получения информации.
* https://github.com/AleksanderPopov/CPP-open/blob/main/Transport%20catalogue/main.cpp

После запуска необходимо указать количество загружаемых данных. После этого ввести необходимые данные.
Дальше указать количество запросов, и сами запросы к данным.
Пример использования в файле https://github.com/AleksanderPopov/CPP-open/blob/main/Transport%20catalogue/test_data.txt

Вторая версия для работы с JSON форматом.
* https://github.com/AleksanderPopov/CPP-open/blob/main/Transport%20catalogue/main_json.cpp
На вход подается JSON с необходимыми данными. На выход JSON с ответами на запросы.
Примеры входных https://github.com/AleksanderPopov/CPP-open/blob/main/Transport%20catalogue/input.json
      и выходных https://github.com/AleksanderPopov/CPP-open/blob/main/Transport%20catalogue/output.json файлов.
 * вариант запроса main_json.exe <input.json 1>output.json 2>cerr.txt

Планы развития программы:
* Вывод карты маршрутов на основе данных SVG формата. Необходимая библиотека по формированию SVG разработана и протестирована.
* Сирелизация данных, для возможности "клиент серверного обмена
* Совмещение разных режимов работы программы задаваемых параметрами командной строки при запуске.
