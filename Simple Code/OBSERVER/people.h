#pragma once

#include <string>

class Person;

class PersonObserver {
public:
    virtual void OnSatisfactionChanged(Person& /*person*/, int /*old_value*/, int /*new_value*/) 
    {
    }

protected:
    ~PersonObserver() = default;
};

class Person {
public:
    Person(const std::string& name, int age) : name_(name),age_(age)
    {
    }

    const std::string& GetName() const {
        return name_;
    }

    void SetObserver(PersonObserver* observer) {
        observer_ = observer;
    }

    int GetAge() const {
        return age_;
    }

    virtual void Dance() {
        int old = satisfaction_;
        dance_count_++;
        satisfaction_ ++;
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old, satisfaction_);
        }        
    }

    virtual void LiveADay() {

    }

    int GetDanceCount() const {
         return dance_count_;
    }

    int GetSatisfaction() const {
        return satisfaction_;
    }

    virtual ~Person() = default;

protected:
    int satisfaction_ = 100;
    std::string name_;
    PersonObserver* observer_ = nullptr;
    int age_;
    int dance_count_ = 0;
private:
    
};

class Worker : public Person {
public:
    Worker(const std::string& name, int age) : Person(name,age)
    {       
    }

    void Dance() override {
        int old = satisfaction_;
        dance_count_++;
        if (age_ > 30 && age_ < 40) {
            satisfaction_ = satisfaction_ + 2;
        }
        else
        {
            satisfaction_++;
        }   
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old, satisfaction_);
        }
    }

    void LiveADay() override{
        Work();
    }

    void Work() {
        int old = satisfaction_;
        work_++;
        satisfaction_ = satisfaction_ - 5;
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old, satisfaction_);
        }
    }

    int GetWorkDone() const {       
        return work_;
    }

private:
    int work_ = 0;
};

class Student : public Person{
public:
    Student(const std::string& name, int age) : Person(name,age)
    {       
    }

    void LiveADay() override{      
        Study();
    }

    void Study() {        
        int old = satisfaction_;
        knoledge_++;
        satisfaction_ = satisfaction_ - 3;
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old, satisfaction_);
        }
    }

    int GetKnowledgeLevel() const {       
        return knoledge_;
    }

private:
    int knoledge_=0;
};