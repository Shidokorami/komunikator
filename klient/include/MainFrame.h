#pragma once
#ifndef MAINFRAME_H
#define MAINFRAME_H
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <sqlite3.h>
#include <map>
#include <pthread.h>
#include <functional>
#include <nlohmann/json.hpp>
#include "ClientLogic.h"
#include "LoginFrame.h"
#include "RegisterFrame.h"
#include "CustomEvents.h"

using json = nlohmann::json;
class MainFrame : public wxFrame{
public:
    MainFrame(const wxString& title);

private:
    LoginFrame* loginFrame;
    RegisterFrame* registerFrame;
    ClientLogic* clientLogic;
    sqlite3* database;
    int rc;

    wxNotebook* listPanel;
    wxPanel* groupPanel;
    wxPanel* friendsPanel;
    wxPanel* rightPanel;
    wxScrolledWindow* chatPanel;
    wxSizer* listsSizer;
    wxSizer* rightSizer;
    wxSplitterWindow* splitter;
    wxListBox* listBox;
    wxTextCtrl* controla;
    int currChatID;
    std::map<int, wxScrolledWindow*> chatWindowsMap;
    std::map<int, wxBoxSizer*> chatSizerMap;
    std::map<wxString, int> groupChatsMap;

    void setupFrames();
    void setupMainFrame();
    void createControls();
    void setupChatWindows();
    void onRegisterRequest(wxCommandEvent& evt);
    void onBackToLoginRequest(wxCommandEvent& evt);
    void onLogInSucces(wxCommandEvent& evt);
    void onChatListboxChange(wxCommandEvent& evt);
    void bindEventRequests();
    void readGroupchatList(wxArrayString& choice);
    void readMessagesFromChat(wxScrolledWindow* window, wxBoxSizer* sizer, int chatID);
    void textFieldEnter(wxCommandEvent& event);
    void handleRequest(std::string message);
    void handleIncomingMessage(json data);
    static void* messageListen(void* arg);
};

#endif