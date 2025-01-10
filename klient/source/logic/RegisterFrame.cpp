#include "../../include/RegisterFrame.h"

 RegisterFrame::RegisterFrame(const wxString& title, wxFrame* parent, ClientLogic* logic): wxFrame(parent, wxID_ANY, title){
    clientLogic = logic;
    mainFrame = parent;
    createControls();
    setupSizers();
    setupHandlers();

 }

 void RegisterFrame::createControls(){
    panel = new wxPanel(this);
    wxFont headlineFont(wxFontInfo(wxSize(0, 36)).Bold());

    headlineText = new wxStaticText(panel, wxID_ANY, "SIGN UP");
    headlineText->SetFont(headlineFont);

    usernameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    passwordCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    usernameText = new wxStaticText(panel, wxID_ANY, "Username:");
    passwordText = new wxStaticText(panel, wxID_ANY, "Password:");

    registerButton = new wxButton(panel, wxID_ANY, "Sign Up");
    backButton = new wxButton(panel, wxID_ANY, "Return to Log In");


 }

 void RegisterFrame::setupSizers(){

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
    mainSizer->AddSpacer(10);
    mainSizer->Add(registerButton, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(10);
    mainSizer->Add(backButton, wxSizerFlags().CenterHorizontal());

    wxGridSizer* outerSizer = new wxGridSizer(1);
    outerSizer->Add(mainSizer, wxSizerFlags().Border(wxALL, 25).Expand());

    panel->SetSizer(outerSizer); // Okno o stałym rozmiarze
    this->SetMinSize(wxSize(600, 400));  // Minimalny rozmiar okna
    this->SetMaxSize(wxSize(600, 400));  // Maksymalny rozmiar okna (żeby nie można było go zwiększyć)

 }

 void RegisterFrame::setupHandlers(){
    registerButton->Bind(wxEVT_BUTTON, &RegisterFrame::onRegisterButtonClicked, this);
    backButton->Bind(wxEVT_BUTTON, &RegisterFrame::onBackButtonClicked, this);
 }

 void RegisterFrame::onRegisterButtonClicked(wxCommandEvent& evt){
    std::string sentUsername = (std::string)usernameCtrl->GetLineText(0);
    std::string sentPassword = (std::string)passwordCtrl->GetLineText(0);

    usernameCtrl->Clear();
    passwordCtrl->Clear();

    std::cout << "Username: " << sentUsername << std::endl;
    std::cout << "Password: " << sentPassword << std::endl; 

    clientLogic->Register(sentUsername, sentPassword);

 }

 void RegisterFrame::onBackButtonClicked(wxCommandEvent& evt){
    sendDynamicEvent(mainFrame, BACK_TO_LOGIN_REQUEST);
 }