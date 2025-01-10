#pragma once
#ifndef REGISTERFRAME_H
#define REGISTERFRAME_H
#include <wx/wx.h>
#include "ClientLogic.h"
#include "CustomEvents.h"


class MainFrame;

class RegisterFrame : public wxFrame
{
public:
    RegisterFrame(const wxString& title, wxFrame* parent, ClientLogic* logic);

private:
    
    wxPanel* panel;
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxStaticText* headlineText;
    wxStaticText* usernameText;
    wxStaticText* passwordText;
    wxButton* registerButton;
    wxButton* backButton;
    ClientLogic* clientLogic;
    wxFrame* mainFrame;

    void createControls();
    void setupSizers();
    void setupHandlers();

    void onRegisterButtonClicked(wxCommandEvent& evt);
    void onBackButtonClicked(wxCommandEvent& evt);

};

#endif