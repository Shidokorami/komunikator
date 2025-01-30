#include "../../include/MainFrame.h"
#include <wx/wx.h>


MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title, wxPoint(100,100), wxDefaultSize){
    rc = sqlite3_open("./resources/resource/klientDB.db", &database);

    if (rc != SQLITE_OK) {
    std::cerr << "Błąd otwierania bazy danych: " << sqlite3_errmsg(database) << std::endl;

    }
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
    clientLogic->getGroupchatsList();
    clientLogic->getMessages();
    setupMainFrame();
    

}

void MainFrame::onChatListboxChange(wxCommandEvent& evt){
    int selectedIndex = listBox->GetSelection();
    wxString wybrany = listBox->GetString(selectedIndex);
    int chat_id = groupChatsMap[wybrany];
    wxScrolledWindow* newScrlWindow = chatWindowsMap[chat_id];
    wxScrolledWindow* oldScrlWindow = chatWindowsMap[currChatID];
    oldScrlWindow->Hide();
    newScrlWindow->Show();
    rightSizer->Layout();
    currChatID = chat_id;


}

void MainFrame::setupMainFrame(){
    
    createControls();
    pthread_t thread;
    pthread_create(&thread, nullptr, MainFrame::messageListen, this);
    pthread_detach(thread);
    Show();
}
void  MainFrame::setupChatWindows(){
    for (auto it =  groupChatsMap.begin(); it !=  groupChatsMap.end(); ++it) {
        wxScrolledWindow* scrlWindow = new wxScrolledWindow(rightPanel, wxID_ANY, wxDefaultPosition, wxSize(500,300));
        scrlWindow->Hide();
        scrlWindow->SetBackgroundColour(wxColor(0, 128, 128));
        scrlWindow->SetScrollRate(5, 5);
        rightSizer->Add(scrlWindow, 2, wxEXPAND | wxALL, 5);
        wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);

        readMessagesFromChat(scrlWindow, chatSizer, it->second);
        scrlWindow->SetSizer(chatSizer);
        scrlWindow->Layout();
        chatWindowsMap.insert({it->second, scrlWindow});
        chatSizerMap.insert({it->second, chatSizer});
    };
}

void MainFrame::createControls(){

    splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    listPanel = new wxNotebook(splitter, wxID_ANY, wxDefaultPosition, wxSize(200,100));
    listPanel->SetBackgroundColour(wxColor(255, 0, 0));

    groupPanel = new wxPanel(listPanel, wxID_ANY);
    friendsPanel = new wxPanel(listPanel, wxID_ANY);

    rightPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition);
    rightSizer = new wxBoxSizer(wxVERTICAL);
    rightPanel->SetSizer(rightSizer);
    wxArrayString choices;
    readGroupchatList(choices);
    
    setupChatWindows();
    
    controla = new wxTextCtrl(rightPanel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 80), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    rightSizer->Add(controla, 1, wxEXPAND | wxALL, 5);
    controla->Bind(wxEVT_TEXT_ENTER, &MainFrame::textFieldEnter, this);
    auto firstChat = *chatWindowsMap.begin();
    currChatID = firstChat.first;
    wxScrolledWindow* firstChatWindow = firstChat.second;
    firstChatWindow->Show();
    rightSizer->Layout();
    
   
    listBox = new wxListBox(groupPanel, wxID_ANY, wxPoint(200, 200), wxSize(100, -1), choices);
    listBox->Bind(wxEVT_LISTBOX, &MainFrame::onChatListboxChange, this);


    listPanel->AddPage(groupPanel, "Chats");
    listPanel->AddPage(friendsPanel, "Friends");


    splitter->SetMinimumPaneSize(200);
    splitter->SplitVertically(listPanel, rightPanel);


   
}


void MainFrame::readGroupchatList(wxArrayString& choices){
    sqlite3_stmt* stmt;
    std::string sql = "SELECT NAME, CHAT_ID FROM GROUPCHATS ORDER BY NAME";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        const unsigned char*  chat_name = sqlite3_column_text(stmt, 0);
        int chat_id = sqlite3_column_int(stmt, 1);
        wxString chatNameStr(reinterpret_cast<const char*>(chat_name));
        groupChatsMap.insert({chat_name, chat_id});
        choices.Add(chatNameStr);
    }

    sqlite3_finalize(stmt);

}

void MainFrame::readMessagesFromChat(wxScrolledWindow* window, wxBoxSizer* sizer, int chatID){
    sqlite3_stmt* stmt;
    std::string sql = "SELECT CONTENT, SENDER_NAME FROM MESSAGES WHERE CHAT_ID = ? ORDER BY MESS_ID ASC";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
    std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
    return;
    }

    rc = sqlite3_bind_int(stmt, 1, chatID);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        const unsigned char*  mess = sqlite3_column_text(stmt, 0);
        const unsigned char*  sender = sqlite3_column_text(stmt, 1);
        wxString messStr(reinterpret_cast<const char*>(mess));
        wxString senderStr(reinterpret_cast<const char*>(sender));
        wxStaticText* text = new wxStaticText(window, wxID_ANY, senderStr + ": " + messStr);
        sizer->Add(text);
    }


}

void MainFrame:: textFieldEnter(wxCommandEvent& event){
    wxString wxMess= controla->GetValue().Trim();
    int chat_id = currChatID;
    if(wxMess.IsEmpty()){
        return;
    }

    std::string mess = wxMess.ToStdString();

    controla->Clear();

    clientLogic->sendMessage(chat_id, mess);

}

void* MainFrame::messageListen(void* arg){
    MainFrame* frame = static_cast<MainFrame*>(arg);

    while(true){
        std::string message = frame->clientLogic->messageListener();
        frame->handleRequest(message);
    }
}

void MainFrame::handleRequest(std::string message){
    std::cout <<"PRZED PARSE: " << message << std::endl;
    json mess = json::parse(message);
    std::string request_type = mess["request"];
    json data = mess["data"];
    std::cout <<"DATA : " << data << std::endl;
    if (request_type =="incoming_message"){
        handleIncomingMessage(data);
    }

}

void MainFrame::handleIncomingMessage(json data){
    int mess_id = data.value("mess_id", -1);
    int chat_id = data.value("chat_id", -1);
    std::string sender_name = data.value("sender_name", "");
    std::string content = data.value("content", "");

    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO MESSAGES(mess_id, chat_id, content, sender_name) VALUES (?,?,?,?);";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 1, mess_id);
    rc = sqlite3_bind_int(stmt, 2, chat_id);
    rc = sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_bind_text(stmt, 4, sender_name.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(database) << std::endl;
    }
    sqlite3_finalize(stmt);

    wxScrolledWindow* window = chatWindowsMap[chat_id];
    wxBoxSizer* sizer = chatSizerMap[chat_id];

    wxStaticText* text = new wxStaticText(window,wxID_ANY, sender_name + ": " + content );
    sizer->Add(text);
    sizer->Layout();
}