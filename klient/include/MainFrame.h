#pragma once
#ifndef MAINFRAME_H
#define MAINFRAME_H
#include <wx/wx.h>
#include <wx/event.h>
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

    void setupFrames();
    void onRegisterRequest(wxCommandEvent& evt);
    void onBackToLoginRequest(wxCommandEvent& evt);
    void bindEventRequests();
};

#endif