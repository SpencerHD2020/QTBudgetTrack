#ifndef BUDGETEXCEPTION_H
#define BUDGETEXCEPTION_H
#include <QException>
#include <QString>

class BudgetException : public QException
{
public:
    BudgetException(QString e) : errorMessage(e) {};
    BudgetException* clone() const { return new BudgetException(*this); };
    void raise() const {throw *this;};
    QString issue() { return errorMessage; };
private:
    QString errorMessage;
};

#endif // BUDGETEXCEPTION_H
