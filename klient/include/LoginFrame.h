#pragma once
#ifndef LOGINFRAME_H
#define LOGINFRAME_H
#include <wx/wx.h>
#include "ClientLogic.h"
class LoginFrame : public wxFrame
{
public:
    LoginFrame(const wxString& title);

private:
    
    wxPanel* panel;
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxStaticText* headlineText;
    wxStaticText* usernameText;
    wxStaticText* passwordText;
    wxButton* loginButton;
    ClientLogic* clientLogic;

    void createControls();
    void setupSizers();
    void setupHandlers();

    void onLoginButtonClicked(wxCommandEvent& evt);

};

#endif