#include <iostream>
#include <windows.h>
#include <string>

using namespace std;
/*
==========================================================================================================================================
ДОМАШНЄ ЗАВДАННЯ 32 тема: успадкування — одиночне, множинне, віртуальний базовий клас.
==========================================================================================================================================
*/

// ==================== РІВЕНЬ 1. ====================

class Employee
{
//  1. Чому поля Employee — protected, а не private? Що доведеться змінити в Developer, якщо зробити їх private?
protected:
    string name;
    int baseSalary;

public:
    Employee(const string& name, int baseSalary) : name(name), baseSalary(baseSalary)
    {}

    string getName() const
    {
        return name;
    }

    int getSalary() const
    {
        return baseSalary;
    }

    void print() const
    {
        cout << name << " (ставка: " << baseSalary << " грн)";
    }
};

class Developer : virtual public Employee
{
private:
    string language;
    int tasksDone;

public:
    //  2. Що станеться, якщо в конструкторі Developer не викликати Employee(name, baseSalary) у списку ініціалізації? Чому?
    //  3. У якому порядку викликаються конструктори Employee і Developer під час створення розробника? А деструктори?
    Developer(const string& name, int baseSalary, const string& language)
        : Employee(name, baseSalary), language(language), tasksDone(0)
    {}

    void completeTask()
    {
        tasksDone++;
    }

    int getTasksDone() const
    {
        return tasksDone;
    }

    /*
        4. Developer::getSalary() і Employee::getSalary() мають однакове ім'я. Яка версія викличеться для developer.getSalary()?
           Як усередині Developer викликати версію Employee?
    */
    int getSalary() const
    {
        return baseSalary + tasksDone * 1000;
    }

    void print() const
    {
        Employee::print();
        cout << ", " << language << ", задач: " << tasksDone << ", зарплата: " << getSalary() << " грн";
    }
};

// ==================== РІВЕНЬ 2. ====================

class Manager : virtual public Employee
{
    /*
          5. (Рівень 2) Manager «є» Employee і «має» розробників. Яке з цих відношень — успадкування, а яке — агрегація?
                        Чому деструктор Manager робить delete[] team, але НЕ видаляє самих розробників?
    */
private:
    Developer** team;
    int teamSize;

public:
    Manager(const string& name, int baseSalary) : Employee(name, baseSalary), team(nullptr), teamSize(0)
    {}

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    ~Manager()
    {
        delete[] team;
    }

    bool addToTeam(Developer* developer)
    {
        if (developer == nullptr)
        {
            return false;
        }

        Developer** newTeam = new Developer * [teamSize + 1];
        for (int i = 0; i < teamSize; i++)
        {
            newTeam[i] = team[i];
        }
        newTeam[teamSize] = developer;

        delete[] team;
        team = newTeam;
        teamSize++;
        return true;
    }

    int getTeamTasks() const
    {
        int sum = 0;
        for (int i = 0; i < teamSize; i++)
        {
            sum += team[i]->getTasksDone();
        }
        return sum;
    }

    int getSalary() const
    {
        return baseSalary + teamSize * 2000;
    }

    void showTeam() const
    {
        cout << "  Команда (" << teamSize << "):" << endl;
        for (int i = 0; i < teamSize; i++)
        {
            cout << "    ";
            team[i]->print();
            cout << endl;
        }
        cout << "  Задач у команди: " << getTeamTasks() << endl;
    }

    void print() const
    {
        Employee::print();
        cout << ", менеджер, зарплата: " << getSalary() << " грн" << endl;
        showTeam();
    }
};

// ==================== РІВЕНЬ 3. ====================

class TeamLead : public Developer, public Manager
{
public:
    
    /*
        6. (Рівень 3) Скільки полів name було б у TeamLead без virtual? Чому без virtual рядок teamLead.getName() не компілюється?
        7. (Рівень 3) Чому TeamLead сам викликає конструктор Employee? Що станеться з викликами Employee(...) у конструкторах Developer
                      і Manager під час створення тімліда?
    */  
    TeamLead(const string& name, int baseSalary, const string& language)
        : Employee(name, baseSalary), Developer(name, baseSalary, language), Manager(name, baseSalary)
    {}

    int getSalary() const
    {
        return Developer::getSalary() + Manager::getSalary() - baseSalary;
    }

    void print() const
    {
        Employee::print();
        cout << ", тімлід, власних задач: " << getTasksDone() << ", зарплата: " << getSalary() << " грн" << endl;
        showTeam();
    }
};

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "===== РІВЕНЬ 1 =====" << endl;
    Developer olena("Олена", 30000, "C++");
    Developer marko("Марко", 28000, "Python");

    for (int i = 0; i < 3; i++)
    {
        olena.completeTask();
    }
    marko.completeTask();

    olena.print();
    cout << endl;
    marko.print();
    cout << endl;
    cout << "Ім'я через успадкований getName(): " << olena.getName() << endl;

    cout << endl << "===== РІВЕНЬ 2 =====" << endl;
    Developer taras("Тарас", 26000, "JavaScript");

    Manager iryna("Ірина", 35000);
    iryna.addToTeam(&olena);
    iryna.addToTeam(&marko);
    iryna.addToTeam(&taras);

    taras.completeTask();                 // задачі після додавання в команду —
    taras.completeTask();                 // менеджер побачить їх, бо зберігає адресу, а не копію

    iryna.print();

    cout << endl << "===== РІВЕНЬ 3 =====" << endl;
    TeamLead andriy("Андрій", 40000, "C++");
    andriy.completeTask();
    andriy.completeTask();
    andriy.addToTeam(&olena);
    andriy.addToTeam(&taras);

    andriy.print();
    cout << "Ім'я тімліда: " << andriy.getName() << endl;   // одне ім'я — завдяки virtual

    return 0;
}

/*

  1. Чому поля Employee — protected, а не private? Що доведеться змінити в Developer, якщо зробити їх private?
  2. Що станеться, якщо в конструкторі Developer не викликати Employee(name, baseSalary) у списку ініціалізації? Чому?
  3. У якому порядку викликаються конструктори Employee і Developer під час створення розробника? А деструктори?
  4. Developer::getSalary() і Employee::getSalary() мають однакове ім'я. Яка версія викличеться для developer.getSalary()?
     Як усередині Developer викликати версію Employee?
  5. (Рівень 2) Manager «є» Employee і «має» розробників. Яке з цих відношень — успадкування, а яке — агрегація?
     Чому деструктор Manager робить delete[] team, але НЕ видаляє самих розробників?
  6. (Рівень 3) Скільки полів name було б у TeamLead без virtual? Чому без virtual рядок teamLead.getName()
     не компілюється?
  7. (Рівень 3) Чому TeamLead сам викликає конструктор Employee? Що станеться з викликами Employee(...)
     у конструкторах Developer і Manager під час створення тімліда?

  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ» (еталон для розбору)

  1. 

  2. 

  3. 

  4. 

  5. 

  6. 

  7. 

*/
