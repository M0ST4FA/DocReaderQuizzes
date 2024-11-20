#pragma once

#include <QtWidgets/QWidget>
#include "ui_ez4ms.h"
#include "googlesso.h"
#include "googleformapi.h"
#include "updateformdialog.h"
#include "additemdialog.h"
#include <memory>

class ez4ms : public QWidget
{
    Q_OBJECT

public:
    ez4ms(QWidget *parent = nullptr);
    ~ez4ms();

protected slots:

    void on_authenticateBtn_clicked();
    void on_removeTokensBtn_clicked();
    void on_requestContentBtn_clicked();
    void on_deleteContentBtn_clicked();

    void on_createFormBtn_clicked();

    void on_updateFormBtn_clicked();

    void on_addMCQBtn_clicked();

    void on_parseFileBtn_clicked();

    void tokenChanged();

private:
    using GoogleFormsAPI = m0st4fa::forms::GoogleFormsAPI;
    using Form = m0st4fa::forms::Form;

    // Ui
    Ui::ez4ms *ui;

    // Credentials
    QString m_clientId;
    QString m_clientSecret;
    QString m_scope{ "https://www.googleapis.com/auth/forms.body https://www.googleapis.com/auth/drive https://www.googleapis.com/auth/drive.file https://www.googleapis.com/auth/forms.responses.readonly" };

    // Networking
    GoogleFormsAPI* m_api;
    GoogleSSO* m_sso;
    std::shared_ptr<Form> m_form = nullptr;
};
