#include "../../include/MainFrame.h"
#include <wx/wx.h>


MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title, wxPoint(100,100), wxDefaultSize){
    
    bindEventRequests();
    clientLogic = new ClientLogic("127.0.0.1", 1100);
    setupFrames();
}

void MainFrame::setupFrames(){
    loginFrame = new LoginFrame("Login", this, clientLogic);
    loginFrame->Center(wxBOTH);
    loginFrame->Show();
}

void MainFrame::bindEventRequests(){
    Bind(REGISTER_REQUESTED, &MainFrame::onRegisterRequest, this);
    Bind(BACK_TO_LOGIN_REQUEST, &MainFrame::onBackToLoginRequest, this);
    Bind(LOGIN_SUCCESS, &MainFrame::onLogInSucces, this);
}

void MainFrame::onRegisterRequest(wxCommandEvent& evt){
    loginFrame->Hide();
    std::cout << "Schowano login" << std::endl;
    registerFrame = new RegisterFrame("Register", this, clientLogic);
    std::cout << "Utworzono register" << std::endl;
    registerFrame->Center(wxBOTH);
    registerFrame->Show();
}

void MainFrame::onBackToLoginRequest(wxCommandEvent& evt){
    registerFrame->Destroy();
    loginFrame->Center(wxBOTH);
    loginFrame->Show();
}

void MainFrame::onLogInSucces(wxCommandEvent& evt){
    loginFrame->Destroy();
    setupMainFrame();

}

void MainFrame::setupMainFrame(){
    createControls();
    Show();
}

void MainFrame::createControls(){
    splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    listPanel = new wxNotebook(splitter, wxID_ANY, wxDefaultPosition, wxSize(200,100));
    listPanel->SetBackgroundColour(wxColor(255, 0, 0));

    groupPanel = new wxPanel(listPanel, wxID_ANY);
    friendsPanel = new wxPanel(listPanel, wxID_ANY);

    wxArrayString choices;
    choices.Add("Jajko");
    choices.Add("Fajko");

    wxStaticText* text1 = new wxStaticText(groupPanel, wxID_ANY, "To jest zakładka 1", wxPoint(20, 20));
    wxStaticText* text2 = new wxStaticText(friendsPanel, wxID_ANY, "To jest zakładka 2", wxPoint(20, 20));
    wxListBox* listBox = new wxListBox(groupPanel, wxID_ANY, wxPoint(200, 200), wxSize(100, -1), choices);


    listPanel->AddPage(groupPanel, "Chats");
    listPanel->AddPage(friendsPanel, "Friends");

    chatPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxSize(200,100));
    chatPanel->SetBackgroundColour(wxColor(0, 128, 128));

    splitter->SetMinimumPaneSize(200);
    splitter->SplitVertically(listPanel, chatPanel);


   
}