#include "../../include/LoginFrame.h"
#include "../../include/CustomEvents.h"
#include <wx/wx.h>


LoginFrame::LoginFrame(const wxString& title, wxFrame* parent, ClientLogic* logic): wxFrame(parent, wxID_ANY, title){
     clientLogic = logic;
     mainFrame = parent;
     if (clientLogic->connectToServer() == false){
         std::cerr <<"Cannot connect to the server!" << std::endl;
     }
     else
     {
        std::cout << "Connected to the server." << std::endl;
     }
    createControls();
    setupSizers();
    setupHandlers();

}

void LoginFrame::createControls(){
    panel = new wxPanel(this);
    wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold());

    headlineText = new wxStaticText(panel, wxID_ANY, "LOG IN");
    headlineText->SetFont(headlineFont);

    usernameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    passwordCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    usernameText = new wxStaticText(panel, wxID_ANY, "Username:");
    passwordText = new wxStaticText(panel, wxID_ANY, "Password:");
    registerText = new wxStaticText(panel, wxID_ANY, "Doesn't have account? Sign up:");

    loginButton = new wxButton(panel, wxID_ANY, "Login");
    registerButton = new wxButton(panel, wxID_ANY, "Sign Up");


    CreateStatusBar();

}

void LoginFrame::setupSizers(){

    panel->Layout();

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(headlineText, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(25);

    wxGridSizer* credSizer = new wxGridSizer(2, 2, wxSize(0,10));
    

    credSizer->Add(usernameText, wxSizerFlags());
    credSizer->Add(usernameCtrl, wxSizerFlags().Proportion(1));
    usernameCtrl->SetMinSize(wxSize(200, -1));
    credSizer->Add(passwordText, wxSizerFlags());
    credSizer->Add(passwordCtrl, wxSizerFlags().Proportion(1));
    passwordCtrl->SetMinSize(wxSize(200, -1));

    mainSizer->Add(credSizer, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(5);
    mainSizer->Add(loginButton, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(10);
    mainSizer->Add(registerText, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(5);
    mainSizer->Add(registerButton, wxSizerFlags().CenterHorizontal());

    wxGridSizer* outerSizer = new wxGridSizer(1);
    outerSizer->Add(mainSizer, wxSizerFlags().Border(wxALL, 25).Expand());

    panel->SetSizer(outerSizer);
 // Okno o stałym rozmiarze
    this->SetMinSize(wxSize(600, 400));  // Minimalny rozmiar okna
    this->SetMaxSize(wxSize(600, 400));  // Maksymalny rozmiar okna (żeby nie można było go zwiększyć)
}

void LoginFrame::setupHandlers(){
    loginButton->Bind(wxEVT_BUTTON, &LoginFrame::onLoginButtonClicked, this);
    registerButton->Bind(wxEVT_BUTTON, &LoginFrame::onRegisterButtonClicked, this);
}

void LoginFrame::onLoginButtonClicked(wxCommandEvent& evt){

    std::string sentUsername = (std::string)usernameCtrl->GetLineText(0);
    std::string sentPassword = (std::string)passwordCtrl->GetLineText(0);

    usernameCtrl->Clear();
    passwordCtrl->Clear();

    std::cout << "Username: " << sentUsername << std::endl;
    std::cout << "Password: " << sentPassword << std::endl; 

    bool isLoginSuccess = clientLogic->Login(sentUsername, sentPassword);
    if(isLoginSuccess){
        std::cout << "SUCCESS" << std::endl;
        sendDynamicEvent(mainFrame, LOGIN_SUCCESS);
    }

}

void LoginFrame::onRegisterButtonClicked(wxCommandEvent& evt){
    sendDynamicEvent(mainFrame, REGISTER_REQUESTED);

}