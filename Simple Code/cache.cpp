#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <map>

template <typename Key, typename Value, typename ValueFactoryFn>
class Cache {
public:
    // Создаёт кэш инициализированный фабрикой значений, созданной по умолчанию
    Cache() = default;

    explicit Cache(ValueFactoryFn value_factory) : value_factory_(value_factory)
    {        
    }

    std::shared_ptr<Value> GetValue(const Key& key) {
        std::shared_ptr<Value> s_ptr = value_[key].lock();
        if (s_ptr) {
            return s_ptr;
        }
        auto sp_ptr = value_factory_(key);
        value_[key] = sp_ptr;
        return sp_ptr;

    }
private:
    std::map<Key, std::weak_ptr<Value>> value_;
    ValueFactoryFn value_factory_;
};

// Пример объекта, находящегося в кэше
class Object {
public:
    explicit Object(std::string id)
        : id_(std::move(id))  //
    {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been created"sv << endl;
    }

    const std::string& GetId() const {
        return id_;
    }

    ~Object() {
        using namespace std;
        cout << "Object '"sv << id_ << "' has been destroyed"sv << endl;
    }

private:
    std::string id_;
};

using ObjectPtr = std::shared_ptr<Object>;

struct ObjectFactory {
    ObjectPtr operator()(std::string id) const {
        return std::make_shared<Object>(std::move(id));
    }
};

void Test1() {
    using namespace std;
    ObjectPtr alice1;
    // Кэш, объектов Object, создаваемых при помощи ObjectFactory,
    // доступ к которым предоставляется по ключу типа string
    Cache<string, Object, ObjectFactory> cache;

    // Извлекаем объекты Alice и Bob
    alice1 = cache.GetValue("Alice"s);
    auto bob = cache.GetValue("Bob"s);
    // Должны вернуться два разных объекта с правильными id
    assert(alice1 != bob);
    assert(alice1->GetId() == "Alice"s);
    assert(bob->GetId() == "Bob"s);

    // Повторный запрос объекта Alice должен вернуть существующий объект
    auto alice2 = cache.GetValue("Alice"s);
    assert(alice1 == alice2);

    // Указатель alice_wp следит за жизнью объекта Alice
    weak_ptr alice_wp{ alice1 };
    alice1.reset();
    assert(!alice_wp.expired());
    cout << "---"sv << endl;
    alice2.reset();
    // Объект Alice будет удалён, так как на него больше не ссылаются shared_ptr
    assert(alice_wp.expired());
    cout << "---"sv << endl;
    // Объект Bob будет удалён, после разрушения указателя bob

    alice1 = cache.GetValue("Alice"s);  // объект 'Alice' будет создан заново
    cout << "---"sv << endl;
}

struct Book {
    Book(std::string title, std::string content)
        : title(std::move(title))
        , content(std::move(content)) {
    }

    std::string title;
    std::string content;
};

class BookLoader {
public:
    using BookStore = std::unordered_map<std::string, std::string>;

    explicit BookLoader(const BookStore& store, size_t& load_count) : store_(store), load_count_(load_count)
    {
    }

    std::shared_ptr<Book> operator()(const std::string& title) const {
            if (store_.count(title) == 0) {
                throw std::out_of_range("Not implemented");
            }
            auto s_ptr = std::make_shared<Book>(title,store_.at(title));
            load_count_++;
            return s_ptr;
    }

private:
    const BookStore& store_;
    size_t& load_count_;
};

void Test2() {
    using namespace std;
    // Хранилище книг.
    BookLoader::BookStore books{
        {"Sherlock Holmes"s,
         "To Sherlock Holmes she is always the woman. I have seldom heard him mention her under any other name."s},
        {"Harry Potter"s, "Chapter 1. The boy who lived. ...."s},
    };
    using BookCache = Cache<string, Book, BookLoader>;

    size_t load_count = 0;
    // Создаём кэш, который будет использщовать BookLoader для загрузки книг из хранилища books
    BookCache book_cache{ BookLoader{books, load_count} };

    // Загруженная книга должна содержать данные из хранилища
    auto book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book1);
    assert(book1->title == "Sherlock Holmes"s);
    assert(book1->content == books.at(book1->title));
    assert(load_count == 1);

    // Повторный запрос книги должен возвращать закешированное значение
    auto book2 = book_cache.GetValue("Sherlock Holmes"s);
    assert(book2);
    assert(book1 == book2);
    assert(load_count == 1);

    weak_ptr<Book> weak_book{ book1 };
    assert(!weak_book.expired());
    book1.reset();
    book2.reset();
    // Книга удаляется, как только на неё перестают ссылаться указатели вне кэша
    assert(weak_book.expired());

    book1 = book_cache.GetValue("Sherlock Holmes"s);
    assert(load_count == 2);
    assert(book1);

    try {
        book_cache.GetValue("Fifty Shades of Grey"s);
        // BookLoader выбросит исключение при попытке загрузить несуществующую книгу
        assert(false);
    }
    catch (const std::out_of_range&) {
        // Всё нормально. Такой книги нет в книгохранилище
    }
    catch (...) {
        cout << "Unexpected exception"sv << endl;
    }
    // Счётчик загрузок не должен обновиться, так как книги нет в хранилище
    assert(load_count == 2);

    // Добавляем книгу в хранилище
    books["Fifty Shades of Grey"s] = "I scowl with frustration at myself in the mirror..."s;

    try {
        auto book = book_cache.GetValue("Fifty Shades of Grey"s);
        // Теперь книга должна быть успешно найдена
        assert(book->content == books.at("Fifty Shades of Grey"s));
    }
    catch (...) {
        assert(false);
    }
    // Счётчик загрузок должен обновиться, так как книги есть в хранилище
    assert(load_count == 3);
}

int main() {
    using namespace std;

    Test1();
    Test2();
    std::cout << "Test complite" << std::endl;

}
