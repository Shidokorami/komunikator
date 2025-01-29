#pragma once
#ifndef MAINFRAME_H
#define MAINFRAME_H
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include "ClientLogic.h"
#include "LoginFrame.h"
#include "RegisterFrame.h"
#include "CustomEvents.h"


class MainFrame : public wxFrame{
public:
    MainFrame(const wxString& title);

private:
    LoginFrame* loginFrame;
    RegisterFrame* registerFrame;
    ClientLogic* clientLogic;

    wxNotebook* listPanel;
    wxPanel* groupPanel;
    wxPanel* friendsPanel;
    wxPanel* chatPanel;
    wxSizer* listsSizer;
    wxSplitterWindow* splitter;

    void setupFrames();
    void setupMainFrame();
    void createControls();
    void onRegisterRequest(wxCommandEvent& evt);
    void onBackToLoginRequest(wxCommandEvent& evt);
    void onLogInSucces(wxCommandEvent& evt);
    void bindEventRequests();
};

#endif