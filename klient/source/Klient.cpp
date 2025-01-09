#include "../include/Klient.h"
#include "../include/LoginFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(Klient);

bool Klient::OnInit() {
    LoginFrame* mainFrame = new LoginFrame("Klient");
    mainFrame ->SetClientSize(800, 600);
    mainFrame ->Center();
    mainFrame ->Show();
    return true;
}