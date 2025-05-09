#pragma once
#ifndef LOGINFRAME_H
#define LOGINFRAME_H
#include <wx/wx.h>
#include "ClientLogic.h"
#include "CustomEvents.h"


class LoginFrame : public wxFrame
{
public:
    LoginFrame(const wxString& title, wxFrame* parent, ClientLogic* logic);

private:
    
    wxPanel* panel;
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxStaticText* headlineText;
    wxStaticText* usernameText;
    wxStaticText* passwordText;
    wxStaticText* registerText;
    wxButton* loginButton;
    wxButton* registerButton;
    ClientLogic* clientLogic;
    wxFrame* mainFrame;

    void createControls();
    void setupSizers();
    void setupHandlers();

    void onLoginButtonClicked(wxCommandEvent& evt);
    void onRegisterButtonClicked(wxCommandEvent& evt);

};

#endif