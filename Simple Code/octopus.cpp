#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Tentacle {
public:
    explicit Tentacle(int id) noexcept
        : id_(id) {
    }

    int GetId() const noexcept {
        return id_;
    }

    Tentacle* GetLinkedTentacle() const noexcept {
        return linked_tentacle_;
    }
    void LinkTo(Tentacle& tentacle) noexcept {
        linked_tentacle_ = &tentacle;
    }
    void Unlink() noexcept {
        linked_tentacle_ = nullptr;
    }

private:
    int id_ = 0;
    Tentacle* linked_tentacle_ = nullptr;
};

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    PtrVector(const PtrVector& other) {
        for (T* p : other.items_) {
            if (p == nullptr) {
                items_.push_back(nullptr);
            }
            else
            {
                items_.push_back(new T(*p));
            }
        }
    }


    PtrVector& operator=(const PtrVector& rhs) {
        if (this != &rhs) {
            PtrVector rhs_copy(rhs);
            swap(this->GetItems(), rhs_copy.GetItems());
        }
        return *this;
    }

    ~PtrVector() {
        for (T* p : items_) {
            delete p;
        }
        items_.clear();
    }

    vector<T*>& GetItems() noexcept {
        return items_;
    }

    vector<T*> const& GetItems() const noexcept {
        return items_;
    }

private:
    vector<T*> items_;

};

class Octopus {
public:
    Octopus()
        : Octopus(8) {
    }
    explicit Octopus(const Octopus& native_octopus) {

        for (size_t i = 0; i < static_cast<size_t>(native_octopus.GetTentacleCount()); ++i) {
            tentacles_.GetItems().push_back(new Tentacle(native_octopus.GetTentacle(i)));

        }
    }

    Octopus& operator=(const Octopus& rhs) {
        if (this != &rhs) {
            Octopus rhs_copy(rhs);
            swap(this->tentacles_.GetItems(), rhs_copy.tentacles_.GetItems());
        }
        return *this;
    }

    explicit Octopus(int num_tentacles) {
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= num_tentacles; ++i) {
                t = new Tentacle(i);
                tentacles_.GetItems().push_back(t);
                t = nullptr;
            }
        }
        catch (const bad_alloc&) {
            Cleanup();
            delete t;
            throw;
        }
    }

    ~Octopus() {
        Cleanup();
    }


    int GetTentacleCount() const noexcept {
        return static_cast<int>(tentacles_.GetItems().size());
    }

    Tentacle& AddTentacle() {
        Tentacle* t = nullptr;
        int i = GetTentacleCount();
        t = new Tentacle(i + 1);
        tentacles_.GetItems().push_back(t);
        return *t;
    }



    const Tentacle& GetTentacle(size_t index) const {
        return *tentacles_.GetItems().at(index);
    }
    Tentacle& GetTentacle(size_t index) {
        return *tentacles_.GetItems().at(index);
    }

private:
    void Cleanup() {
        for (Tentacle* t : tentacles_.GetItems()) {
            delete t;
        }
        tentacles_.GetItems().clear();
    }

    PtrVector<Tentacle> tentacles_;
};

int main() {
    // Проверка присваивания осьминогов
    {
        Octopus octopus1(3);
        // Настраиваем состояние исходного осьминога
        octopus1.GetTentacle(2).LinkTo(octopus1.GetTentacle(1));

        // До присваивания octopus2 имеет своё собственное состояние
        Octopus octopus2(10);

        octopus2 = octopus1;

        // После присваивания осьминогов щупальца копии имеют то же состояние,
        // что и щупальца присваиваемого объекта
        assert(octopus2.GetTentacleCount() == octopus1.GetTentacleCount());
        for (int i = 0; i < octopus2.GetTentacleCount(); ++i) {
            auto& tentacle1 = octopus1.GetTentacle(i);
            auto& tentacle2 = octopus2.GetTentacle(i);
            assert(&tentacle2 != &tentacle1);
            assert(tentacle2.GetId() == tentacle1.GetId());
            assert(tentacle2.GetLinkedTentacle() == tentacle1.GetLinkedTentacle());
        }
    }

    // Проверка самоприсваивания осьминогов
    {
        Octopus octopus(3);
        // Настраиваем состояние осьминога
        octopus.GetTentacle(0).LinkTo(octopus.GetTentacle(1));

        vector<pair<Tentacle*, Tentacle*>> tentacles;
        // Сохраняем информацию о щупальцах осьминога и его копии
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            tentacles.push_back({ &octopus.GetTentacle(i), octopus.GetTentacle(i).GetLinkedTentacle() });
        }

        // Выполняем самоприсваивание
        octopus = octopus;

        // После самоприсваивания состояние осьминога не должно измениться
        assert(octopus.GetTentacleCount() == static_cast<int>(tentacles.size()));
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            auto& tentacle_with_link = tentacles.at(i);
            assert(&octopus.GetTentacle(i) == tentacle_with_link.first);
            assert(octopus.GetTentacle(i).GetLinkedTentacle() == tentacle_with_link.second);
        }
    }
}
