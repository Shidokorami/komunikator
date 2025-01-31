#include "../../include/MainFrame.h"
#include <wx/wx.h>


MainFrame::MainFrame(const wxString& title): wxFrame(nullptr, wxID_ANY, title, wxPoint(100,100), wxDefaultSize){
    rc = sqlite3_open("./resources/resource/klientDB.db", &database);

    if (rc != SQLITE_OK) {
    std::cerr << "Błąd otwierania bazy danych: " << sqlite3_errmsg(database) << std::endl;

    }
    clearDB();
    bindEventRequests();
    clientLogic = new ClientLogic("127.0.0.1", 5000);
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
    clientLogic->getFriendList();
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

void MainFrame::clearDB(){
    sqlite3_stmt* stmt;
    std::string sql = "DELETE FROM GROUPCHATS;";

    rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt); 
    sqlite3_finalize(stmt);

    sql = "DELETE FROM MESSAGES;";

    rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt); 
    sqlite3_finalize(stmt);

    sql = "DELETE FROM FRIENDS;";

    rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt); 
    sqlite3_finalize(stmt);



    
}

void MainFrame::createControls(){

    splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    listPanel = new wxNotebook(splitter, wxID_ANY, wxDefaultPosition, wxSize(200,100));
    listPanel->SetBackgroundColour(wxColor(255, 0, 0));

    groupPanel = new wxPanel(listPanel, wxID_ANY);
    friendsPanel = new wxPanel(listPanel, wxID_ANY);

    createGroupButton = new wxButton(groupPanel, wxID_ANY, "Create groupchat", wxPoint(170, 0));
    createGroupButton->Bind(wxEVT_BUTTON, &MainFrame::createChatButtonClicked, this);

    addFriendButton = new wxButton(friendsPanel, wxID_ANY, "Add friend", wxPoint(170, 0));
    addFriendButton->Bind(wxEVT_BUTTON, &MainFrame::addFriendButtonClicked, this);

    addToChat = new wxButton(friendsPanel, wxID_ANY, "Add to chat", wxPoint(170,50));
    addToChat->Bind(wxEVT_BUTTON, &MainFrame::addToChatButtonClicked, this);

    listPanel->AddPage(groupPanel, "Chats");
    listPanel->AddPage(friendsPanel, "Friends");

    rightPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition);
    rightSizer = new wxBoxSizer(wxVERTICAL);
    rightPanel->SetSizer(rightSizer);
    readGroupchatList();
    readFriendList();
    
    setupChatWindows();
    
    controla = new wxTextCtrl(rightPanel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 80), wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    rightSizer->Add(controla, 1, wxEXPAND | wxALL, 5);

    controla->Bind(wxEVT_TEXT_ENTER, &MainFrame::textFieldEnter, this);
    auto firstChat = *chatWindowsMap.begin();
    currChatID = firstChat.first;
    wxScrolledWindow* firstChatWindow = firstChat.second;
    firstChatWindow->Show();
    rightSizer->Layout();
    
    
    listBox = new wxListBox(groupPanel, wxID_ANY, wxPoint(0, 0), wxSize(150, 400), chatChoices);
    listBox->Bind(wxEVT_LISTBOX, &MainFrame::onChatListboxChange, this);

    friendList = new wxListBox(friendsPanel, wxID_ANY, wxPoint(0, 0), wxSize(150, 400), friendChoices);
    friendList->Bind(wxEVT_LISTBOX, &MainFrame::onFriendListChange, this);

    splitter->SetMinimumPaneSize(200);
    splitter->SplitVertically(listPanel, rightPanel);


   
}


void MainFrame::readGroupchatList(){
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
        chatChoices.Add(chatNameStr);
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
    
    json mess = json::parse(message);
    std::string request_type = mess["request"];
    json data = mess["data"];
    if (request_type =="incoming_message"){
        handleIncomingMessage(data);
    }
    else if (request_type == "added_to_chat"){
        handleAddedToChat(data);
    }
    else if (request_type == "added_as_friend"){
        handleAddedToFriends(data);
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

void MainFrame::createChatButtonClicked(wxCommandEvent& event){
    createGroupDialog = new wxDialog(this, wxID_ANY, "Create groupchat", wxDefaultPosition, wxSize(300, 200));
    createGroupSizer = new wxBoxSizer(wxVERTICAL);

    createGroupTextCtrl = new wxTextCtrl( createGroupDialog, wxID_ANY, "", wxDefaultPosition, wxSize(290, 30), wxTE_PROCESS_ENTER);
    createGroupSizer->Add(createGroupTextCtrl);

    createGroupSizerButton = new wxBoxSizer(wxHORIZONTAL);
    
    createGroupAcceptButton = new wxButton( createGroupDialog, wxID_ANY, "Create");
    createGroupSizerButton->Add(createGroupAcceptButton, 0, wxALL | wxCENTER, 10);
    createGroupAcceptButton->Bind(wxEVT_BUTTON, &MainFrame::createNewChat, this);

    createGroupSizer->Add(createGroupSizerButton, 0, wxALL | wxCENTER, 10);

    createGroupDialog->SetSizerAndFit(createGroupSizer);

    createGroupDialog->ShowModal();
}

void MainFrame::createNewChat(wxCommandEvent& event){
    std::string name = createGroupTextCtrl->GetValue().Trim().ToStdString();
    createGroupTextCtrl->Clear();
    clientLogic->createNewChat(name);

    createGroupDialog->EndModal(wxID_OK);
}

void MainFrame::handleAddedToChat(json data){
    int chat_id = data.value("chat_id", -1);
    std::string name = data.value("name", "");

    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO GROUPCHATS(chat_id, name) VALUES (?,?);";

    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    rc = sqlite3_bind_int(stmt, 1, chat_id);
    rc = sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(database) << std::endl;
    }
    sqlite3_finalize(stmt);

    groupChatsMap.insert({name, chat_id});
    wxString wxstring(name.c_str(), wxConvUTF8);
    listBox->Append(wxstring);

     wxScrolledWindow* scrlWindow = new wxScrolledWindow(rightPanel, wxID_ANY, wxDefaultPosition, wxSize(500,300));
    scrlWindow->Hide();
    scrlWindow->SetBackgroundColour(wxColor(0, 128, 128));
    scrlWindow->SetScrollRate(5, 5);
    
    rightSizer->Detach(controla);
    rightSizer->Add(scrlWindow, 2, wxEXPAND | wxALL, 5);
    rightSizer->Add(controla, 1, wxEXPAND | wxALL, 5);
    wxBoxSizer* chatSizer = new wxBoxSizer(wxVERTICAL);

    scrlWindow->SetSizer(chatSizer);
    scrlWindow->Layout();
    rightSizer->Layout();
    rightPanel->Layout();
    chatWindowsMap.insert({chat_id, scrlWindow});
    chatSizerMap.insert({chat_id, chatSizer});

    
}

void MainFrame::readFriendList(){
    sqlite3_stmt* stmt;
    std::string sql = "SELECT USERNAME FROM FRIENDS ORDER BY USERNAME";
    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        const unsigned char*  friend_name = sqlite3_column_text(stmt, 0);
        wxString friendNameStr(reinterpret_cast<const char*>(friend_name));
        friendChoices.Add(friendNameStr);
    }

    sqlite3_finalize(stmt);

}

void MainFrame::addFriendButtonClicked(wxCommandEvent& event){
    addFriendDialog = new wxDialog(this, wxID_ANY, "Add friend", wxDefaultPosition, wxSize(300, 200));
    addFriendSizer = new wxBoxSizer(wxVERTICAL);

    addFriendTextCtrl = new wxTextCtrl( addFriendDialog, wxID_ANY, "", wxDefaultPosition, wxSize(290, 30), wxTE_PROCESS_ENTER);
    addFriendSizer->Add(addFriendTextCtrl);

    addFriendSizerButton = new wxBoxSizer(wxHORIZONTAL);
    
    addFriendAcceptButton = new wxButton( addFriendDialog, wxID_ANY, "Create");
    addFriendSizerButton->Add(addFriendAcceptButton, 0, wxALL | wxCENTER, 10);
    addFriendAcceptButton->Bind(wxEVT_BUTTON, &MainFrame::addFriend, this);

    addFriendSizer->Add(addFriendSizerButton, 0, wxALL | wxCENTER, 10);

    addFriendDialog->SetSizerAndFit(addFriendSizer);

    addFriendDialog->ShowModal();
}

void MainFrame::addFriend(wxCommandEvent& event){
    std::cout<< "W addFriend" << std::endl;
    std::string name = addFriendTextCtrl->GetValue().Trim().ToStdString();
    addFriendTextCtrl->Clear();
    std::cout<< "Przed logic" << std::endl;
    clientLogic->addFriend(name);
    std::cout<< "Po logic" << std::endl;
    addFriendDialog->EndModal(wxID_OK);
}

void MainFrame::handleAddedToFriends(json data){
    std::cout << "W handle added" << std::endl;
    std::string name = data.value("name", "");

    sqlite3_stmt* stmt;
    std::string sql = "INSERT INTO FRIENDS(USERNAME) VALUES (?);";

    int rc = sqlite3_prepare_v2(database, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(database) << std::endl;
        return;
    }

    rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Błąd insertu rekordu: " << sqlite3_errmsg(database) << std::endl;
    }
    sqlite3_finalize(stmt);

    wxString wxstring(name.c_str(), wxConvUTF8);

    friendList->Append(wxstring);


}

void MainFrame::onFriendListChange(wxCommandEvent& evt){
    int selectedIndex = friendList->GetSelection();
    wxString wybrany = friendList->GetString(selectedIndex);
    currFriendName = wybrany.ToStdString();
}

void MainFrame::addToChatButtonClicked(wxCommandEvent& event){
    int chat_id = currChatID;
    std::string name = currFriendName;
    clientLogic->addToChat(chat_id, name);
    

}
