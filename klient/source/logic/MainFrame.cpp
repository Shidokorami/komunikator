#include "../../include/MainFrame.h"
#include <wx/wx.h>


MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title, wxPoint(100,100), wxDefaultSize, wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxICONIZE){
    
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